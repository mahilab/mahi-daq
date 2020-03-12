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
#include <string>
#include <unordered_map>

namespace mahi {
namespace daq {

/// Utility class for creating Quanser configuration strings. 
/// Because there is some overlap in configuration option strings
/// between DAQs, we have included all options in this single class. 
/// However, not all options apply to every DAQ, and attempting 
/// to pass a non-supported option will result in an error.
/// See the "Applies to" heading to determine if your DAQ supports an
/// option. Note that we have not implemented all possible options,
/// so use the "special" string to pass options that do not appear here.
/// For  more information, see Quanser's documentation:
/// Q2Usb: https://docs.quanser.com/quarc/documentation/q2_usb.html
/// Q8Usb: https://docs.quanser.com/quarc/documentation/q8_usb.html
/// QPIDe: https://docs.quanser.com/quarc/documentation/qpid_e.html
struct QuanserOptions {

    // Structs, enums, etc.
    enum class UpdateRate { Normal, Fast };
    enum class EncoderDirection { Nonreversed = 0, Reversed = 1 };
    enum class EncoderFilter { Unfiltered = 0, Filtered = 1 };
    enum class EncoderDetection { High = 0, Low = 1 };
    enum class EncoderReload { NoReload = 0, OnPulse = 1 };
    enum class LedMode { None, Auto, User };
    enum class DoMode { None, Digital, Pwm };
    enum class AoMode { VoltageMode  = 0, CurrentMode2 = 1, CurrentMode1 = 2, CurrentMode0 = 3, ControlMode2 = 4, ControlMode1 = 5, ControlMode0 = 6};
    struct AoPrams { AoMode ch_mode; double ch_kff, ch_a0, ch_a1, ch_a2, ch_b0, ch_b1, ch_post; };
    
    /// Makes the Quanser formatted options string
    std::string str();    
    /// Return  all options to default
    void clear();

    /// Function:       Set normal (1kHz) or fast (8kHz) board updating
    /// Quanser option: update_rate
    /// Applies to:     Q2Usb, Q8Usb
    UpdateRate update_rate = UpdateRate::Normal;
    /// Function:       Decimates reads/writes (e.g. if 10 the DAQ will only read/write every 10 samples)
    /// Quanser option: decimation
    /// Applies to:     Q2Usb, Q8Usb
    unsigned int decimation = 1;
    /// Function:       Set LED mode on Q2-USB
    /// Quanser option: led
    /// Applies to:     Q2Usb
    LedMode led = LedMode::None;
    /// Function:       Sets digital I/O mode on line 0 of Q2-USB
    /// Quanser option: d0
    /// Applies to:     Q2Usb
    DoMode d0;
    /// Function:       Sets digital I/O mode on line 1 of Q2-USB
    /// Quanser option: d1
    /// Applies to:     Q2Usb
    DoMode d1;
    /// Function:       Reverses an encoder direction
    /// Quanser option: encX_dir
    /// Applies to:     Q8Usb, QPid
    std::map<ChanNum, EncoderDirection> encX_dir;
    /// Function:       Enables filtering on an encoder
    /// Quanser option: encX_filter
    /// Applies to:     Q8Usb, QPid
    std::map<ChanNum, EncoderFilter> encX_filter;
    /// Function:       Encoder channel A detection state
    /// Quanser option: encX_a
    /// Applies to:     Q8Usb, QPid
    std::map<ChanNum, EncoderDetection> encX_a;
    /// Function:       Encoder channel B detection state
    /// Quanser option: encX_b
    /// Applies to:     Q8Usb, QPid
    std::map<ChanNum, EncoderDetection> encX_b;
    /// Function:       Encoder channel Z detection state
    /// Quanser option: encX_z
    /// Applies to:     Q8Usb, QPid
    std::map<ChanNum, EncoderDetection> encX_z;
    /// Function:       Enables encoder reloading to 0 on index pulse
    /// Quanser option: encX_reload
    /// Applies to:     Q8Usb, QPid
    std::map<ChanNum, EncoderReload> encX_reload;
    /// Function:       Sets minimum detectable encoder velocity
    /// Quanser option: encX_velocity
    /// Applies to:     Q8Usb, QPid
    std::map<ChanNum, double> encX_velocity;
    /// Function:       Enables PWM output on Q8-USB
    /// Quanser option: pwmX_en
    /// Applies to:     Q8Usb
    std::map<ChanNum, bool> pwmX_en;
    /// Function:       Sets analog mode and parameters (consult Quanser docs)
    /// Quanser option: chX_mode, chX_kff, chX_a0, chX_a1, chX_a2, chX_b0, chX_b1, chX_post
    /// Applies to:     Q8Usb
    std::map<ChanNum, AoPrams> aoX_params;
    /// Function:       Set special user specific string options
    /// Quanser option: N/A
    /// Applies to:     Any
    std::string special;
};

} // namespace daq
} // namespace mahi

