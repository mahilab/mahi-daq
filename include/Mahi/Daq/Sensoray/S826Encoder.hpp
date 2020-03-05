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

#include <mahi/daq/Encoder.hpp>

namespace mahi {
namespace daq {

class S826;

class S826Encoder : public Encoder {
public:

    class Channel;

    bool update_channel(ChanNum channel_number) override;
    bool reset_count(ChanNum channel_number, int count) override;
    bool set_quadrature_factor(ChanNum channel_number, QuadFactor factor) override;

    /// Returns values per second for all channels
    std::vector<double>& get_values_per_sec();

    /// Returns valus per second for a single channel
    double get_value_per_sec(ChanNum channel_number);

    /// Performs conversion to positions using #factors_ and #counts_per_unit
    const std::vector<double>& get_velocities();

    /// Performs conversion to position using #factors_ and #counts_per_unit
    double get_velocity(ChanNum channel_number);

    /// Gets the hardware based timestamps of the most recent update
    std::vector<double>& get_timestamps();

    /// Gets the hardware based timestamp of the most recent update for single channel
    double get_timestamp(ChanNum channel_number);

    /// Returns a S826Encoder::Channel
    Channel get_channel(ChanNum channel_number);

    /// Returns multiple S826Encoder::Channels
    std::vector<Channel> get_channels(const ChanNums& channel_numbers);

    /// Returns a S826Encoder::Channel
    Channel operator[](ChanNum channel_number);

    /// Returns multiple S826Encoder::Channels
    std::vector<Channel> operator[](const ChanNums& channel_numbers);

private:

    friend class S826;

    /// Private constructor used by S826
    S826Encoder(S826& s826);

    /// Called by S826 when it opens
    bool on_open();

private:

    S826& s826_;
    
    Buffer<double> timestamps_;
    Buffer<double> values_per_sec_;
    Buffer<double> velocities_;

    S826Encoder( const S826Encoder& ) = delete; // non construction-copyable
    S826Encoder& operator=( const S826Encoder& ) = delete; // non copyable

public:

    /// Encapsulates and S826Encoder channel (can be used as a PositionSensor or VelocitySensor)
    class Channel : public Encoder::Channel {
    public:
        /// Default constructor. Creates invalid channel
        Channel();

        /// Creates a valid channel.
        Channel(S826Encoder* module, ChanNum channel_number);

        /// Inherit assignment operator for setting
        using ChannelBase<int>::operator=;

        /// Gets the encoder counts per second if available
        double get_value_per_sec();

        /// Gets the encoder velocity if available
        double get_velocity();

        /// Gets the timestamp of the encoder read
        double get_timestamp();
    private:
        double velocity_;
    };

};

} // namespace daq
} // namespace mahi