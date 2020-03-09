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

class DaqBase;
class ModuleInterfaceBase;
class Readable;
class Writeable;

/// A DAQ Module. A DAQ interface may expose one or more Modules.
/// Modules should expose one type of I/O functionality of the DAQ, but
/// may use any number of ModuleInterfaces to implement the functionality,
/// e.g. separate ModuleInterfaces for buffers and registry settings. 
class Module : public util::Device {
public:
    /// Default Constructor (creates an invlaid empty Module)
    Module(DaqBase& daq);
    /// Gets the vector of channel numbers this Module maintains
    const ChanNums& channels() const;
    /// Gets the vector of channel numbers this Module maintains in the internal representation
    const ChanNums& channels_internal() const;
    /// Checks if a channel number is a number defined on this Module.
    bool valid_channel(ChanNum ch, bool quiet = false) const;
    /// Checks if the size of a vector equals the number of channels
    bool valid_count(std::size_t size, bool quiet = false) const;
    /// Returns buffer index associated with channel number
    std::size_t index(ChanNum ch) const;
protected:
    /// Sets the public facing channel numbers this Module maintains.
    /// You may choose to make this public in your own implementation.
    void set_channel_numbers(const ChanNums& chs);
    /// Adds a public facing channel number to current channel numbers
    /// You may choose to make this public in your own implementation.
    void add_channel_number(ChanNum ch);
    /// Removes a public facing channel number from current channel numbers
    /// You may choose to make this public in your own implementation.
    void remove_channel_number(ChanNum ch);
    /// Transforms a public facing channel number to the internal representation.
    /// Passes through by default. Override if your DAQ API channel indexing is 
    /// different from the interface indexing you you want clients to use. 
    virtual ChanNum transform_channel_number(ChanNum public_facing) const;
private:
    /// Updates the channel Map and notifies ModuleInterfaces
    void update_map();
    /// Updates internal channel numbers
    void update_internal();
private:
    friend ModuleInterfaceBase;
    friend Readable;
    friend Writeable;
    ChanNums m_chs_public;                      ///< The public facing channel numbers
    ChanNums m_chs_internal;                    ///< The internal facing channel numbers
    ChanMap  m_ch_map;                          ///< Maps a public facing channel number to a buffer index position
    std::vector<ModuleInterfaceBase*> m_ifaces; ///< Interfaces maintained  by this Module
    DaqBase& m_daq;                             ///< This Module's parent Daq
};

} // namespace daq
} // namespace mahi

// #include <Mahi/Daq/Detail/Module.inl>
