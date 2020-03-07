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
#include <Mahi/Daq/ChannelBase.hpp>
#include <Mahi/Daq/Module.hpp>

namespace mahi {
namespace daq {

//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an Input only Module
template <typename T>
class Input : public Module<T> {
public:
    class Channel;

    /// Default Constructor (creates an invlaid empty Input Module)
    Input();

    /// Constructor with specified channel numbers
    Input(const ChanNums& channel_numbers);

    /// Default destructor
    virtual ~Input();

    /// Gets a handle to a channel on this module
    Channel channel(ChanNum channel_number);

    /// Encapsulates a Module channel
    class Channel : virtual public ChannelBase<T> {
    public:
        /// Default constructor. Creates invalid channel
        Channel();

        /// Creates a valid channel.
        Channel(Input* module, ChanNum channel_number);

        /// Inherit assignment operator for setting
        using ChannelBase<T>::operator=;
    };
};

//==============================================================================
// TYPEDEFS
//==============================================================================

typedef Input<Voltage> AnalogInput;
typedef Input<Logic>   DigitalInput;

} // namespace daq
} // namespace mahi

#include <Mahi/Daq/Detail/Input.inl>
