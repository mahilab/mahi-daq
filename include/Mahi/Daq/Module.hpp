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

/// Defines non-templated Module functions/members
class Module : public util::Device {
public:

    /// Default Constructor (creates an invlaid empty Module)
    Module(DaqBase& daq);

    /// Gets the vector of channel numbers this Module maintains
    const ChanNums& channel_numbers() const;

    /// Returns the number of channels on this Module
    std::size_t channel_count() const;

    /// Checks if a channel number is a number defined on this Module.
    bool valid_channel(ChanNum channel_number, bool quiet = false) const;

    /// Checks if the size of a vector equals the number of channels
    bool valid_count(std::size_t size, bool quiet = false) const;

    /// Returns vector index associated with channel number
    std::size_t index(ChanNum channel_number) const;

protected:

    /// Sets the channel numbers this Module maintains
    void set_channel_numbers(const ChanNums& channel_numbers);

    /// Adds a channel number to current channel numbers
    void add_channel_number(ChanNum channel_number);

    /// Removes a channel number from current channel numbers
    void remove_channel_number(ChanNum channel_number);

private:

    friend class ModuleInterfaceBase;

    /// Updates the channel Map and notifies Registries
    void update_map();

private:
    friend Readable;
    friend Writeable;
    ChanNums channel_numbers_;                  ///< The channel numbers used by this ModuleInterface
    ChanMap  channel_map_;                      ///< Maps a channel number with a vector index position
    std::vector<ModuleInterfaceBase*> interfaces_; ///< Objects needed by this Module
    DaqBase& m_daq;                             ///< This modules Daq
};

} // namespace daq
} // namespace mahi

// #include <Mahi/Daq/Detail/Module.inl>
