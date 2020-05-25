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
#include <Mahi/Util/Device.hpp>
#include <Mahi/Util/Event.hpp>
#include <map>

namespace mahi {
namespace daq {

class Daq;
class BufferBase;
class Readable;
class Writeable;

/// A Module implements subfunctions of a DAQ
class Module : public util::NonCopyable {
public:
    /// Constructor.
    Module(Daq& daq);
    /// Destructor.
    virtual ~Module() {}
    /// Returns the Module's name.
    const std::string& name() const;
    /// Returns const reference to this Module's parent DAQ
    Daq& daq() const;

protected:
    /// Called when the DAQ opens.
    virtual bool on_daq_open() { return true; }
    /// Called when the DAQ closes.
    virtual bool on_daq_close() { return true; }
    /// Called when the DAQ enables.
    virtual bool on_daq_enable() { return true; }
    /// Called when the DAQ disables.
    virtual bool on_daq_disable() { return true; }
    /// Set the Module's name
    void set_name(const std::string& name);

private:
    friend Daq;
    Daq&        m_daq;   ///< This Module's parent Daq
    std::string m_name;  ///< This Module's string name
};

typedef std::vector<std::pair<ChanNums, ChanNums>> SharedPins;

/// ChannelModules expose one type of array-like I/O functionality of the DAQ
class ChanneledModule : public Module {
public:
    /// Constructor. Sets the allowed channels, but does not set the current channels.
    ChanneledModule(Daq& daq, const ChanNums& allowed);
    /// Sets the channel numbers this Module maintains. The channels requested
    /// must be a subset of the Module's allowed channels. If this Module shares
    /// pins/channels with another, those Modules will have their pins reclaimed.
    /// Understand that if you are holding references to any Buffer values, this
    /// may, and likely will, invalidate those references, so use this only on
    /// startup before pulling references. Channel Initialization and finalization 
    /// functionality can be added by connecting to on_gain_ and on_free_channels.
    bool set_channels(const ChanNums& chs);
    /// Gets the channel numbers this Module is currently maintaining.
    const ChanNums& channels() const;
    /// Gets the list of channels allowed on the Module.
    const ChanNums& channels_allowed() const;
    /// Gets the channel numbers this Module maintains in the internal representation.
    const ChanNums& channels_internal() const;
    //// Returns true if this Module shares pins with another.
    bool shares_pins() const;
    /// Shared pins data structure
protected:
    /// Converts a public facing channel number to the internal representation.
    /// Passes through by default. Override if your DAQ API channel indexing is
    /// different from the interface indexing you you want clients to use.
    virtual ChanNum convert_channel(ChanNum public_facing) const;   
    /// Called when new channels have been gained
    virtual bool on_gain_channels(const ChanNums& chs) { return true; }
    /// Called when old channels have been freed
    virtual bool on_free_channels(const ChanNums& chs) { return true; }

protected:
    /// Connect Buffer on_read(s)
    template <typename B, typename F>
    inline void connect_read(B& buffer, F func) {
        buffer.on_read.connect(func);
    }
    /// Connect Buffer on_write(s)
    template <typename B, typename F>
    inline void connect_write(B& buffer, F func) {
        buffer.on_write.connect(func);
    }
    /// Connect Buffer post_read(s)
    template <typename B, typename F>
    inline void connect_post_read(B& buffer, F func) {
        buffer.post_read.connect(func);
    }
    /// Connect Buffer post_write(s)
    template <typename B, typename F>
    inline void connect_post_write(B& buffer, F func) {
        buffer.post_write.connect(func);
    }

private:
    friend Daq;
    static void create_shared_pins(ChanneledModule* a, ChanneledModule* b, SharedPins shares_pins);

private:
    friend BufferBase;
    ChanNums m_chs_allowed;   ///< The allowed public facing channel numbers
    ChanNums m_chs_public;    ///< The current public facing channel numbers
    ChanNums m_chs_internal;  ///< The current internal facing channel numbers
    ChanMap  m_ch_map;        ///< Maps a public facing channel number to a buffer index position
    std::vector<BufferBase*> m_buffs;  ///< Buffers maintained  by this Module
};

}  // namespace daq
}  // namespace mahi