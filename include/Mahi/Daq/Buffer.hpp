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
#include <Mahi/Daq/Module.hpp>
#include <functional>

namespace mahi {
namespace daq {

using util::Event;
using util::CollectorBooleanAnd;

/// Base class for Module array types
class BufferBase : util::NonCopyable {
public:
    /// Constructor
    BufferBase(ChanneledModule& module);
    /// Returns const reference to this interfaces owning Module
    inline ChanneledModule& module() const { return m_module; }

protected:
    friend ChanneledModule;
    /// Called by Module when channel numbers change
    virtual void remap(const ChanMap& old_map, const ChanMap& new_map) = 0;
    /// Returns internal channel number
    inline ChanNum intern(ChanNum public_facing) {
        return m_module.convert_channel(public_facing);
    }
    /// Returns buffer index associated with channel number.
    inline std::size_t index(ChanNum channel_number) const {
        return m_module.m_ch_map.at(channel_number);
    }
    /// Checks if a channel number is a number currently maintained on this Module.
    bool valid_channel(ChanNum ch, bool quiet = false) const;
    /// Checks if #size equals the number of maintained channels.
    bool valid_count(std::size_t size, bool quiet = false) const;
private:
    ChanneledModule& m_module;  ///< pointer to parent module
};

/// Templated Buffer
template <typename T>
class Buffer : public BufferBase {
public:
    /// Typedef  of the interfaces's value Type for templating purposes
    typedef T Type;
    typedef std::vector<T> BufferType;
    /// Constructor
    Buffer(ChanneledModule& module, T default_value);
    /// Overload stream operator
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const Buffer<U>& buf);

protected:
    /// Returns a constant reference to the entire internal buffer
    const std::vector<T>& buffer() const { return m_buffer; }
    /// Returns a non-constant reference to the entire internal buffer
    std::vector<T>& buffer() { return m_buffer; }
    /// Returns a constant reference to buffer element indexed by channel number (write access)
    const T& buffer(ChanNum ch) const { return m_buffer[index(ch)]; }
    /// Returns a non-const reference to buffer element index by channel number (read access)
    T& buffer(ChanNum ch) { return m_buffer[index(ch)]; }

private:
    /// Called by parent Module when its channel numbers change
    void remap(const ChanMap& old_map, const ChanMap& new_map) override;

private:
    BufferType m_buffer;   ///< raw buffer
    T          m_default;  ///< default value
};

/// Flags a Buffer as a Readable, i.e. one that physically reads from the DAQ
class Readable {
public:
    /// Constructor
    Readable(ChanneledModule& module);
    /// Read implementation that will be called from a read_all
    virtual bool read() = 0;
    /// If true, read will be called when a read_all call is made
    bool read_with_all;
};

/// Flags a Buffer as a Writeable, i.e. one that physically writes to the DAQ
class Writeable {
public:
    /// Constructor
    Writeable(ChanneledModule& module);
    /// Write implementation that will be called from a write_all
    virtual bool write() = 0;
    /// If true, write will be called when a write_all call is made
    bool write_with_all;
};

//==============================================================================
// MIXINS
//==============================================================================

// These mixins allow you to inject public and protected functionality into a Buffer<T>
// For instance, they can be combined to expose array like buffering and/or immediate mode
// read/write

/// Mixin this to inject buffer get access into a Buffer<T> (see Io.hpp for examples)
template <typename Base>
class IGet : public Base {
public:
    /// Constructor
    IGet(ChanneledModule& module, typename Base::Type default_value)
        : Base(module, default_value) {}
    /// Buffer read access with operator[] (does NOT validate channel number, invalid numbers will
    /// cause undefined behavior)
    const typename Base::Type& operator[](ChanNum ch) const { return this->buffer(ch); }
    /// Get all buffer values at once
    const typename Base::BufferType& get() const { return this->buffer(); }
    /// Get a copy to a single buffer value (channel number is validated, and returns type default
    /// value if invalid)
    typename Base::Type get(ChanNum ch) const {
        if (this->valid_channel(ch))
            return this->buffer(ch);
        else
            return typename Base::Type();
    }
};

/// Mixin this to inject buffer set/get access into a Buffer<T> (see Io.hpp for examples)
template <typename Base>
class ISet : public IGet<Base> {
public:
    /// Constructor
    ISet(ChanneledModule& module, typename Base::Type default_value)
        : IGet<Base>(module, default_value) {}
    /// Buffer write access with operator[] (does NOT validate channel number, invalid numbers will
    /// cause undefined behavior)
    typename Base::Type& operator[](ChanNum ch) { return this->buffer(ch); }
    /// Set all buffer values at once (does size check)
    void set(const typename Base::BufferType& values) {
        if (this->valid_count(values.size()))
            this->buffer() = values;
    }
    /// Sets a single channel. The channel must be valid.
    bool set(ChanNum ch, typename Base::Type value) {
        if (this->valid_channel(ch)) {
            this->buffer(ch) = value;
            return true;
        }
        return false;
    }
    /// Sets a subset of channels. The channels and values passed must have the same size,
    /// and all channel numbers must be valid.
    bool set(const ChanNums& chs, const typename Base::BufferType& values) {
        if (chs.size() != values.size())
            return false;
        for (auto& ch : chs) {
            if (!this->valid_channel(ch))
                return false;
        }
        for (int i = 0; i < chs.size(); ++i)
            this->buffer(chs[i]) = values[i];
        return true;
    }
};

/// Mixin this to inject an immediate read interface into a Buffer<T> (see Io.hpp for examples)
template <typename Base>
class IRead : public Base, public Readable {
public:
    /// Constructor
    IRead(ChanneledModule& module, typename Base::Type default_value)
        : Base(module, default_value), Readable(module), on_read(nullptr), post_read(nullptr) {}
    /// Immediately reads values into the software buffer.
    /// Returns true for success, false otherwise. Overrides Readable::read.
    virtual bool read() override {
        if (on_read.emit(&this->module().channels_internal()[0], &this->buffer()[0],
                         this->module().channels_internal().size())) {
            post_read.emit(&this->module().channels_internal()[0], &this->buffer()[0],
                           this->module().channels_internal().size());
            return true;
        }
        return false;
    }
    /// Immediately reads a single channel into the software buffer.
    /// Returns true for success, false otherwise.
    bool read(ChanNum ch) {
        ChanNum intern_ch = this->intern(ch);
        if (this->valid_channel(ch) && on_read.emit(&intern_ch, &this->buffer(ch), 1)) {
            post_read.emit(&intern_ch, &this->buffer(ch), 1);
            return true;
        }
        return false;
    }

protected:
    friend ChanneledModule;
    /// Connect to this Event to read all requested channel numbers into the buffer.
    /// The channel numbers passed will be the internal representation (see
    /// Module::transform_channels).
    Event<bool(const ChanNum*, typename Base::Type*, std::size_t), CollectorBooleanAnd> on_read;
    /// Connect to this Event if you need to update other values after a successful read.
    /// The channel numbers passed will be the internal representation (see
    /// Module::transform_channels).
    Event<void(const ChanNum*, const typename Base::Type*, std::size_t)> post_read;
};

/// Mixin this to inject an immediate write interface into a Buffer<T> (see Io.hpp for examples)
template <typename Base>
class IWrite : public Base, public Writeable {
public:
    /// Constructor
    IWrite(ChanneledModule& module, typename Base::Type default_value)
        : Base(module, default_value), Writeable(module), on_write(nullptr) {}
    /// Immediately writes the values currently stored in the software buffer.
    /// Returns true for success, false otherwise. Overrides Writeable::write.
    virtual bool write() override {
        if (on_write.emit(&this->module().channels_internal()[0], &this->buffer()[0],
                          this->module().channels_internal().size())) {
            post_write.emit(&this->module().channels_internal()[0], &this->buffer()[0],
                            this->module().channels_internal().size());
            return true;
        }
        return false;
    }
    /// Immediately writes the passed vector. It's size must be equal to the number of channels.
    /// Returns true for success, false otherwise.
    bool write(const typename Base::BufferType& values) {
        if (this->valid_count(values.size()) &&
            on_write.emit(&this->module().channels_internal()[0], &values[0],
                          this->module().channels_internal().size())) {
            this->buffer() = values;
            post_write.emit(&this->module().channels_internal()[0], &this->buffer()[0],
                            this->module().channels_internal().size());
            return true;
        }
        return false;
    }
    /// Immediately writes a single channel value. The channel number must be valid.
    /// Returns true for success, false otherwise.
    bool write(ChanNum ch, typename Base::Type value) {
        ChanNum intern_ch = this->intern(ch);
        if (this->valid_channel(ch) && on_write.emit(&intern_ch, &value, 1)) {
            this->buffer(ch) = value;
            post_write.emit(&intern_ch, &this->buffer(ch), 1);
            return true;
        }
        return false;
    }
    /// Immediately writes a subset of channels (up to 64). The channel numbers must be valid and
    /// chs and values must be the same size. Returns true for success, false otherwise.
    bool write(const ChanNums& chs, const typename Base::BufferType& values) {
        std::size_t n = chs.size() > 64 ? 64 : chs.size();
        ChanNum     intern_chs[64];
        for (std::size_t i = 0; i < n; ++i) {
            if (!this->valid_channel(chs[i]))
                return false;
            intern_chs[i] = this->intern(chs[i]);
        }
        if (chs.size() == values.size() && on_write.emit(intern_chs, &values[0], n)) {
            for (std::size_t i = 0; i < chs.size(); ++i)
                this->buffer(chs[i]) = values[i];
            post_write.emit(intern_chs, &values[0], n);
            return true;
        }
        return false;
    }

protected:
    friend ChanneledModule;
    /// Connect to this Event to write all requested channel numbers from the buffer.
    /// The channel numbers passed will be the internal representation (see
    /// Module::convert_channel).
    Event<bool(const ChanNum*, const typename Base::Type*, std::size_t), CollectorBooleanAnd>  on_write;
    /// Connect to this Event if you need to update other values after a successful read
    /// The channel numbers passed will be the internal representation (see
    /// Module::convert_channel).
    Event<void(const ChanNum*, const typename Base::Type*, std::size_t)> post_write;
};

/// Exposes the protected members of Protected to Beneficiary
template <class Protected, class Beneficiary>
class Friend : public Protected {
    using Protected::Protected;
    friend Beneficiary;
};

}  // namespace daq
}  // namespace mahi

#include <Mahi/Daq/Detail/Buffer.inl>
