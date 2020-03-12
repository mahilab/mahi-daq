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
#include <Mahi/Daq/Quanser/QuanserDaq.hpp>
#include <Mahi/Daq/Quanser/QuanserAI.hpp>
#include <Mahi/Daq/Quanser/QuanserAO.hpp>
#include <Mahi/Daq/Quanser/QuanserDI.hpp>
#include <Mahi/Daq/Quanser/QuanserDO.hpp>
#include <Mahi/Daq/Quanser/QuanserPWM.hpp>
#include <Mahi/Daq/Quanser/QuanserEncoder.hpp>
#include <Mahi/Daq/Quanser/QuanserWatchdog.hpp>

namespace mahi {
namespace daq {

class Q2Usb final : public QuanserDaq {
public:
    /// Constructor. Automatically opens communication.
    Q2Usb();
    /// Destructor. First disables if enabled, then closes if open.
    ~Q2Usb();
    /// The Quanser Q2-USB supports two 12-bit single-ended analog inputs with
    /// a ±10V range. Hence analog channel numbers range from 0 to 1.The range
    /// of the analog inputs is fixed at ±10V.
    Fused<QuanserAI,Q2Usb> AI;
    /// The Quanser Q2-USB supports two 12-bit single-ended analog outputs with
    /// a ±10V range. Hence analog channel numbers range from 0 to 1.
    Fused<QuanserAO,Q2Usb> AO;
    /// The Quanser Q2-USB supports 8 bidirectional digital I/O lines. Hence 
    /// digital input channel numbers range from 0 to 7. A digital I/O line 
    /// cannot be used as an input and output at the same time.
    Fused<QuanserDI,Q2Usb> DI;
    /// The Quanser Q2-USB supports 8 bidirectional digital I/O lines and 1 LED. 
    /// Hence digital output channel numbers range from 0 to 8. A digital I/O 
    /// line cannot be used as an input and output at the same time. The LED 
    /// can be controlled as digital output 8.
    Fused<QuanserDO,Q2Usb> DO;
    /// The Quanser Q2-USB driver supports two PWM output channels. The PWM outputs
    /// are shared with digital I/O lines 0 and 1. he PWM base frequencies can range 
    /// from 2.384Hz up to 40MHz. It only supports QuanserPwm::Mode::DutyCycle.
    Fused<QuanserPwm,Q2Usb> PWM;
    /// The Quanser Q2-USB supports two quadrature encoder inputs with 16-bit 
    /// count values. Hence encoder channel numbers range from 0 to 1. It only
    //  supports  the 4X quadrature mode. Since the Quanser Q2-USB has 16-bit 
    /// counters, valid initial count values range from -32,768 to +32,767.
    Fused<QuanserEncoder,Q2Usb> encoder;
    /// Q2-USB watchdog timer. The timer may be programmed with any interval 
    /// between 125us and 8.1 seconds. Set AO, DO, and PWM expire values you
    /// desire on those Modules. They will be set automatically through 
    /// hardware in the event of a software failure (i.e. watchdog timout).
    Fused<QuanserWatchdog,Q2Usb> watchdog;
};

} // namespace daq
} // namespace mahi