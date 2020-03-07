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

/// Encapsulates an incremental optical encoder module with 32-bit precision
class Encoder : public Module<int> {
public:
    class Channel;

    /// Default Constructor (creates an invlaid empty Encoder)
    Encoder();

    /// Constructor with specified channel numbers
    Encoder(const ChanNums& channel_numbers);

    /// Default destructor
    virtual ~Encoder();

    /// Calls the DAQ's API to set a single encoder counter
    virtual bool reset_count(ChanNum channel_number, int count) = 0;

    /// This function should call the DAQ's API to set all encoder counters
    virtual bool reset_counts(const std::vector<int>& counts);

    /// This function should call the DAQ's API to set the quadrature factor on all encoder channels
    virtual bool set_quadrature_factors(const std::vector<QuadFactor>& factors);

    /// This function should call the DAQ's API to set the quadrature factor on a single channel
    virtual bool set_quadrature_factor(ChanNum channel_number, QuadFactor factor);

    /// Zeros a single encoder channel
    bool zero_channel(ChanNum channel_number);

    /// Zeros all encoder channels (calls reset_counts({0,...,0}))
    bool zero();

    /// Sets the units per count on a single encoder channel
    void set_units_per_count(ChanNum channel_number, double units_per_count);

    /// Sets the units per count of all encoder channels.
    void set_units_per_count(const std::vector<double>& units_per_count);

    /// Performs conversion to position using #factors_ and #counts_per_unit
    double get_position(ChanNum channel_number);

    /// Performs conversion to positions using factors_ and counts_per_unit
    const std::vector<double>& get_positions() const;
    std::vector<double>& get_positions();

    /// Returns values per second for all channels
    const std::vector<double>& get_values_per_sec() const;
    std::vector<double>& get_values_per_sec();

    /// Returns valus per second for a single channel
    double get_value_per_sec(ChanNum channel_number);

    /// Performs conversion to positions using #factors_ and #counts_per_unit
    const std::vector<double>& get_velocities() const;
    std::vector<double>& get_velocities();

    /// Performs conversion to position using #factors_ and #counts_per_unit
    double get_velocity(ChanNum channel_number); 

    /// Returns a Encoder::Channel
    Channel channel(ChanNum channel_number);

    /// Does this incremental encoder have velocity estimation? (false unless override)
    virtual bool has_velocity() const;

protected:

    /// Override to call compute_conversions
    virtual bool on_enable() override;

    /// Precomputes position conversion sclars (i.e. units_per_count_ / factors_)
    void compute_conversions();

protected:

    Buffer<QuadFactor> factors_;         ///< The encoder quadrature factors (default X4)
    Buffer<double> units_per_count_;     ///< The number of counts per unit of travel of the Encoder
    mutable Buffer<double> positions_;   ///< The calculated positions of the Encoder channels
    Buffer<double> values_per_sec_;      ///< The encoder velocities, if supported
    mutable Buffer<double> velocities_;  ///< The calculated velocities of the Encoder channels
    Buffer<double> conversions_;         ///< Conversion scalars used to convert to positions

public:
    /// Encapsulates and Encoder channel (can be used as a PositionSensor)
    class Channel : public ChannelBase<int> {
    public:
        /// Default constructor. Creates invalid channel
        Channel();

        /// Creates a valid channel.
        Channel(Encoder* module, ChanNum channel_number);

        /// Gets the encoder position
        double get_position();

        /// Gets the encoder counts per second if available
        double get_value_per_sec();

        /// Gets the encoder velocity if available
        double get_velocity();

        /// Zeros the encoder count
        bool zero();

        /// Sets the encoder count to a specific value
        bool reset_count(int count);

        /// Sets the encoder units/count
        void set_units_per_count(double units_per_count);

    private:

        /// Sets the encoder quadrature factor
        bool set_quadrature_factor(QuadFactor factor);

    };
};

} // namespace daq
} // namespace mahi
