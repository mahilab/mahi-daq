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
#include <mahi/daq/Types.hpp>
#include <string>

namespace mahi {
namespace daq {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Utility class for creating Quanser configuration strings
class QuanserOptions {
public:
    enum class UpdateRate {
        Normal = 1,  ///< 1 kHz
        Fast   = 2   ///< 8 kHz
    };

    enum class EncoderDirection { Nonreversed = 0, Reversed = 1 };

    enum class EncoderFilter { Unfiltered = 0, Filtered = 1 };

    enum class EncoderDetection { High = 0, Low = 1 };

    enum class EncoderReload { NoReload = 0, OnPulse = 1 };

    enum class AoMode {
        VoltageMode  = 0,
        CurrentMode2 = 1,
        CurrentMode1 = 2,
        CurrentMode0 = 3,
        ControlMode2 = 4,
        ControlMode1 = 5,
        ControlMode0 = 6
    };

    enum class LedMode { Auto = 0, User = 1 };

public:
    QuanserOptions();

    void set_update_rate(UpdateRate update_rate);

    void set_decimation(ChanNum decimation);

    void set_encoder_direction(ChanNum channel_number,
                               EncoderDirection direction);

    void set_encoder_filter(ChanNum channel_number, EncoderFilter filter);

    void set_encoder_detection_a(ChanNum channel_number,
                                 EncoderDetection detection);

    void set_encoder_detection_b(ChanNum channel_number,
                                 EncoderDetection detection);

    void set_encoder_detection_z(ChanNum channel_number,
                                 EncoderDetection detection);

    void set_encoder_reload(ChanNum channel_number, EncoderReload reload);

    void set_encoder_velocity(ChanNum channel_number, double velocity);

    void enable_pwm(ChanNum channel_number, bool enable);
    
    void set_analog_output_mode(ChanNum channel_number,
                                AoMode mode,
                                double kff,
                                double a0,
                                double a1,
                                double a2,
                                double b0,
                                double b1,
                                double post);

    void set_special_option(std::string option);

    void set_led_mode(LedMode mode);

    std::string get_string();

    void clear();

private:
    std::string options_;     ///< the configuration options string
};

} // namespace daq
} // namespace mahi

// http://www.quanser.com/Products/quarc/documentation/q8_usb.html
