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
#include <Mahi/Daq/Io.hpp>
#include <Mahi/Daq/Quanser/QuanserHandle.hpp>
#include <Mahi/Util/Timing/Frequency.hpp>

namespace mahi {
namespace daq {

class QuanserDaq;

/// Quanser PWM output Module
class QuanserPwm : public PwmModuleBasic {
public:
    /// The output mode to be use when write is called
    enum class Mode {
        DutyCycle, /// PWM outputs vary in duty cycle by percentage, with frequency fixed (default).
        Frequency, /// PWM outputs vary in frequency, with duty cycle fixed. 
        Period,    /// PWM outputs vary in period, with duty cycle fixed
        OneShot    /// PWM outputs vary in duty cycle, only a single pulse generated per write
    };
    /// Constructor
    QuanserPwm(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed, 
        std::function<bool(const ChanNums&)> on_gain_custom = nullptr, 
        std::function<bool(const ChanNums&)> on_free_custom = nullptr);
        /// The writing output mode of each channel
    Register<Mode> modes;
    /// The values to default to if the DAQ watchdog expires.
    Register<double> expire_values;
    /// The frequencies to be used in Mode::DutyCycle in Hz
    Register<double> frequencies;
    /// The duty cycles to be use in Mode::Frequency and Mode::Period in range 0 to 1
    Register<double> duty_cycles;
private:
    friend QuanserDaq;
    bool on_gain_channels(const ChanNums& chs) override;
    bool on_free_channels(const ChanNums& chs) override;
    std::function<bool(const ChanNums&)> m_on_gain_custom;
    std::function<bool(const ChanNums&)> m_on_free_custom;
    QuanserHandle& m_h;
};

} // namespace daq
} // namespace mahi