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
    virtual ~Module() { }
    /// Returns the Module's name.
    const std::string& name() const;
    /// Returns reference to this Module's parent DAQ
    Daq& daq() const;
public:
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
    Daq& m_daq;         ///< This Module's parent Daq
    std::string m_name; ///< This Module's string name
};

/// ChannelModules expose one type of array-like I/O functionality of the DAQ
class ChanneledModule : public Module {
public:
    /// Constructor. Sets the allowed channels, but does not set the current channels.
    /// Make an explicit call to set_channels. 
    ChanneledModule(Daq& daq, const ChanNums& allowed);
    /// Sets the channel numbers this Module maintains. The channels requested
    /// must be a subset of the Modue's allowed channels. If this Module shares
    /// pins/channels with another, those Modules will have their pins reclaimed.
    /// Understand that if you are holding references to any buffer values, this 
    /// may, and likely will, invalidate those references, so use this only on 
    // startup before pulling references. 
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
    typedef std::vector<std::pair<ChanNums,ChanNums>> ShareList;
protected:
    /// Transforms a public facing channel number to the internal representation.
    /// Passes through by default. Override if your DAQ API channel indexing is 
    /// different from the interface indexing you you want clients to use. 
    virtual ChanNum transform_channel(ChanNum public_facing) const;
    /// Use this to facilitate pin sharing between ChannelsModules e.g. DIOs 
    /// commonly share pins with w/ PWM, I2C, encoders, etc.
    /// ShareList({{{0},{0,1}},{{1,2},{2}}}) means this Module's channel 0
    /// shares with others's channels 0,1, and this Modules's channels 1,2 
    /// shares with others's channel 2.
    void share_pins_with(ChanneledModule* other, ShareList share_list);
    /// Called when new channels have been gained
    util::Event<bool(const ChanNums&),util::CollectorBooleanAnd> on_gain_channels;
    /// Called when old channels have been freed
    util::Event<bool(const ChanNums&),util::CollectorBooleanAnd> on_free_channels;
private:
    friend BufferBase;
    ChanNums m_chs_allowed;                     ///< The allowed public facing channel numbers
    ChanNums m_chs_public;                      ///< The current public facing channel numbers
    ChanNums m_chs_internal;                    ///< The current internal facing channel numbers
    ChanMap  m_ch_map;                          ///< Maps a public facing channel number to a buffer index position
    std::vector<BufferBase*> m_ifaces; ///< Interfaces maintained  by this Module
};

} // namespace daq
} // namespace mahi