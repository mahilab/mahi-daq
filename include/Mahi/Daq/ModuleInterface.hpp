// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once
#include <Mahi/Daq/Types.hpp>
#include <Mahi/Util/Event.hpp>
#include <Mahi/Util/NonCopyable.hpp>
#include <functional>

namespace mahi {
namespace daq {

class Module;
class ChannelsModule;

/// Base class for Module array types
class ModuleInterfaceBase : util::NonCopyable {
public:
    /// Constructor
    ModuleInterfaceBase(ChannelsModule& module);
protected:
    friend ChannelsModule;
    /// Called by Module when channel numbers change
    virtual void remap_channels(const ChanMap& old_map, const ChanMap& new_map) = 0;
    /// Returns internal channel number
    ChanNum intern(ChanNum public_facing);
protected:
    ChannelsModule& m_module; ///< pointer to parent module
};

/// Templated ModuleInterface
template <typename T>
class ModuleInterface : public ModuleInterfaceBase {
public:
    /// Typedef  of the interfaces's value Type for external use
    typedef T ValueType;
    /// Constructor
    ModuleInterface(ChannelsModule& module, T default_value);
    /// Overload stream operator
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const ModuleInterface<U>& array);
protected:   
    /// Returns a constant reference to the entire internal buffer
    const std::vector<T>& buffer() const { return m_buffer; }
    /// Returns a non-constant reference to the entire internal buffer
    std::vector<T>& buffer() { return m_buffer; }
    /// Returns a constant reference to buffer elemented indexed by channel number (write access)
    const T& buffer(ChanNum ch) const { return m_buffer[m_module.index(ch)]; }
    /// Returns a non-const reference (read access)
    T& buffer(ChanNum ch) { return m_buffer[m_module.index(ch)]; }
private:
    /// Called by ModuleInterface when channel numbers change
    void remap_channels(const ChanMap& old_map, const ChanMap& new_map) override;
private:
    std::vector<T> m_buffer;  ///< raw buffer
    T m_default;              ///< default value
};

/// Flags a ModuleInterface as a Readable, i.e. one that physically reads from the DAQ
class Readable {
public:
    /// Constructor
    Readable(ChannelsModule& module);
    /// Read implementation that will be called from a read_all 
    virtual bool read() = 0;
    /// If true, read will be called when a read_all call is made
    bool read_with_all;
};

/// Flags a ModuleInterface as a Writeable, i.e. one that physically writes to the DAQ
class Writeable {
public:
    /// Constructor
    Writeable(ChannelsModule& module);
    /// Write implementation that will be called from a write_all 
    virtual bool write() = 0;
    /// If true, write will be called when a write_all call is made
    bool write_with_all;
};

//=============================================================================
// MIXINS
//=============================================================================

// These mixins allow you to inject public and private functionality into a ModuleInterface<T>
// For instance, they can be combined to expose array like buffering and/or immediate mode read/write.

/// Mixin this to inject buffer get access into a ModuleInterface<T> (see Io.hpp)
template <typename Base>
class IGet : public Base {
public:
    /// Constructor
    IGet(ChannelsModule& module, typename Base::ValueType default_value) : Base(module, default_value) { }
    /// Buffer read access with operator[] (does NOT validate channel number, invalid numbers will cause undefined behavior)
    const typename Base::ValueType& operator[](ChanNum ch) const { return this->buffer(ch); }
    /// Get all buffer values at once
    const std::vector<typename Base::ValueType>& get() const {
        return this->buffer();
    }
};

/// Mixin this to inject buffer set/get access into a ModuleInterface<T> (see Io.hpp)
template <typename Base>
class ISet : public IGet<Base> {
public:
    /// Constructor
    ISet(ChannelsModule& module, typename Base::ValueType default_value) : IGet<Base>(module, default_value) { }
    /// Buffer write access with operator[] (does NOT validate channel number, invalid numbers will cause undefined behavior)
    typename Base::ValueType& operator[](ChanNum ch) { return this->buffer(ch); }
    /// Set all buffer values at once (does size check)
    void set(const std::vector<typename Base::ValueType>& values) {
        if (this->m_module.valid_count(values.size()))
            this->buffer() = values;
    }
};

/// Mixin this to inject an immediate read interface into a ModuleInterface<T> (see Io.hpp)
template <typename Base>
class IRead : public Base, public Readable {
public:
    /// Constructor
    IRead(ChannelsModule& module, typename Base::ValueType default_value) :
        Base(module, default_value), 
        Readable(module),
        on_read(nullptr), post_read(nullptr)
    { }
    /// Immediately reads values into the software buffer. 
    /// Returns true for success, false otherwise. Overrides Readable::read.
    virtual bool read() override {
        if (on_read.size() > 0 && on_read.emit(&this->m_module.channels_internal()[0], &this->buffer()[0], this->m_module.channels_internal().size()))
        {
            if (post_read.size() > 0)
                post_read.emit(&this->m_module.channels_internal()[0], &this->buffer()[0], this->m_module.channels_internal().size());
            return true;
        }
        return false;
    }

    /// Immediately reads a single channel into the software buffer. 
    /// Returns true for success, false otherwise.
    bool read(ChanNum ch) {
        ChanNum int_ch = this->intern(ch);
        if (this->m_module.valid_channel(ch) && on_read.size() > 0 && on_read.emit(&int_ch, &this->buffer(ch), 1))
        {
            if (post_read.size() > 0)
                post_read.emit(&int_ch, &this->buffer(ch), 1);
            return true;
        }
        return false;
    }
    
protected:
    /// Connect to this Event to read all requested channel numbers into the buffer.
    /// The channel numbers passed will be the internal representation (see Module::transform_channels).
    util::Event<bool(const ChanNum *, typename Base::ValueType *, std::size_t), util::CollectorBooleanAnd> on_read;
    /// Connect to this Event if you need to update other values after a successful read.
    /// The channel numbers passed will be the internal representation (see Module::transform_channels).
    util::Event<void(const ChanNum *, const typename Base::ValueType *, std::size_t)> post_read;
};

/// Mixin this to inject an immediate write interface into a ModuleInterface<T>
template <typename Base>
class IWrite : public Base, public Writeable {
public:
    IWrite(ChannelsModule& module, typename Base::ValueType default_value) : 
        Base(module, default_value), Writeable(module), on_write(nullptr)        
    { }

    /// Immediately writes the values currently stored in the software buffer. 
    /// Returns true for success, false otherwise. Overrides Writeable::write.
    virtual bool write() override {
        if (on_write.size() > 0 && on_write.emit(&this->m_module.channels_internal()[0], &this->buffer()[0], this->m_module.channels_internal().size())) {
            if (post_write.size() > 0)
                post_write.emit(&this->m_module.channels_internal()[0], &this->buffer()[0], this->m_module.channels_internal().size());
            return true;
        }
        return false;
    }

    /// Immediately writes the passed vector. It's size must be equal to the number of channels.
    /// Returns true for success, false otherwise.
    bool write(const std::vector<typename Base::ValueType>& values) {
        if (this->m_module.valid_count(values.size()) && on_write.size() > 0 &&
            on_write.emit(&this->m_module.channels_internal()[0], &values[0], this->m_module.channels_internal().size())) 
        {
            this->buffer() = values;
            if (post_write.size() > 0)
                post_write.emit(&this->m_module.channels_internal()[0], &this->buffer()[0], this->m_module.channels_internal().size());
            return true;
        }
        return false;
    }

    /// Immediately writes a single channel value. The channel number must be valid. 
    /// Returns true for success, false otherwise.
    bool write(ChanNum ch, typename Base::ValueType value) {
        ChanNum int_ch = this->intern(ch);
        if (this->m_module.valid_channel(ch) && on_write.size() > 0 && on_write.emit(&int_ch, &value, 1)) {
            this->buffer(ch) = value;
            if (post_write.size() > 0)
                post_write.emit(&int_ch, &this->buffer(ch), 1);
            return true;
        }
        return false;   
    }
protected:
    /// Connect to this Event to write all requested channel numbers from the buffer.
    /// The channel numbers passed will be the internal representation (see Module::transform_channels).
    util::Event<bool(const ChanNum*, const typename Base::ValueType*, std::size_t), util::CollectorBooleanAnd> on_write;
    /// Connect to this Event if you need to update other values after a successful read
    /// The channel numbers passed will be the internal representation (see Module::transform_channels).
    util::Event<void(const ChanNum *, const typename Base::ValueType *, std::size_t)> post_write;
};

} // namespace daq
} // namespace mahi

#include <Mahi/Daq/Detail/ModuleInterface.inl>
