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
class ModuleInterfaceBase;
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
    /// Gets const reference to this Module's DAQ
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

/// A DAQ interface may expose one or more Modules.
/// Modules should expose one type of I/O functionality of the DAQ, but
/// may use any number of ModuleInterfaces to implement the functionality,
/// e.g. separate ModuleInterfaces for buffers and registry settings. 
class ChannelsModule : public Module {
public:
    /// Constructor.
    ChannelsModule(Daq& daq);
    /// Gets the vector of channel numbers this Module maintains.
    const ChanNums& channels() const;
    /// Gets the vector of channel numbers this Module maintains in the internal representation.
    const ChanNums& channels_internal() const;
    /// Checks if a channel number is a number defined on this Module.
    bool valid_channel(ChanNum ch, bool quiet = false) const;
    /// Checks if the size of a vector equals the number of channels.
    bool valid_count(std::size_t size, bool quiet = false) const;
    /// Returns buffer index associated with channel number.
    std::size_t index(ChanNum ch) const;
    /// Sets the channel numbers this Module maintains. Understand that if you
    /// are holding references to any buffer values, this may, and likely will, 
    /// invalidate those references, so use this only on startup before pulling
    /// references. If this Module share's pins with other Modules, they will
    /// be updated as well if needed.
    void set_channel_numbers(const ChanNums& chs);
    //// Returns true if this Module shares pins
    bool shares_pins();
public:
    /// Transforms a public facing channel number to the internal representation.
    /// Passes through by default. Override if your DAQ API channel indexing is 
    /// different from the interface indexing you you want clients to use. 
    virtual ChanNum transform_channel_number(ChanNum public_facing) const;
    /// Share pins data structure
    typedef std::vector<std::pair<ChanNums,ChanNums>> ShareList;
    /// Use this to facilitate pin sharing between ChannelsModules
    /// #share_pins establishes the channel sharing policy, e.g.
    /// {{{0},{0,1}},{{1,2},{2}}} means a's channel 0 shares with b's 
    /// channels 0,1, and a's channels 1,2 shares with b's channel 2.
    static void share(ChannelsModule* a, ChannelsModule* b, ShareList share_list);
private:
    /// Updates the channel Map and notifies ModuleInterfaces
    void update_map();
    /// Updates internal channel numbers
    void update_internal();
    /// Updates shared pins
    void update_shared();
private:
    friend ModuleInterfaceBase;
    ChanNums m_chs_public;                      ///< The public facing channel numbers
    ChanNums m_chs_internal;                    ///< The internal facing channel numbers
    ChanMap  m_ch_map;                          ///< Maps a public facing channel number to a buffer index position
    std::vector<ModuleInterfaceBase*> m_ifaces; ///< Interfaces maintained  by this Module
};

} // namespace daq
} // namespace mahi

// #include <Mahi/Daq/Detail/Module.inl>
