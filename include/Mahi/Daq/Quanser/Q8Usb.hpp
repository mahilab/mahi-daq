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
#include <Mahi/Daq/Quanser/QuanserPwm.hpp>
#include <Mahi/Daq/Quanser/QuanserEncoder.hpp>
#include <Mahi/Daq/Quanser/QuanserWatchdog.hpp>

namespace mahi {
namespace daq {

/// Implements a Quanser Q8-USB multi-function DAQ device
/// Documentation: https://docs.quanser.com/quarc/documentation/q8_usb.html
class Q8Usb final : public QuanserDaq {
public:
    /// Constructor. Automatically opens communication if #auto_open is true.
    Q8Usb(bool auto_open = true);
    /// Destructor. First disables if enabled, then closes if open.
    ~Q8Usb();
    /// The Quanser Q8-USB supports eight 16-bit differential analog inputs
    /// with a ±10V range. Hence channel numbers range from 0 to 7. 
    /// The range of the analog inputs may be configured as ±5V or ±10V.
    QuanserAI AI;
    /// The Quanser Q8-USB supports eight 16-bit single-ended analog outputs 
    /// with a programmable range. As there are eight outputs, channel 
    /// numbers range from 0 to 7. Valid ranges are 0-5V, 0-10V, 0-10.8V, 
    /// ±5V, ±10V and ±10.8V 
    QuanserAO AO;
    /// The Quanser Q8-USB supports 8 digital input lines and two special
    /// digital input lines. Hence digital input channel numbers range from
    /// 0 to 9. Channels 8 and 9 are EXT_INT and CONVERT respectively. 
    /// We do not enable channels 8 and 9 by default. Digital inputs cannot
    /// be reprogrammed as digital outputs.
    QuanserDI DI;
    /// The Quanser Q8-USB supports 8 digital output lines. Hence channel 
    /// numbers range from 0 to 7. 
    QuanserDO DO;
    /// The Quanser Q8-USB driver supports up to eight PWM output channels. 
    /// The PWM outputs share the same pins as the digital outputs.
    QuanserPwm PWM;
    /// The Quanser Q8-USB supports eight quadrature encoder inputs with 24-bit
    /// count values. Hence encoder channel numbers range from 0 to 7. The 
    /// Quanser Q8-USB supports 0X non-quadrature (count and direction) and 4X 
    /// quadrature. Since the Quanser Q8-USB has 24-bit counters, valid initial 
    /// count values range from -8,388,608 to +8,388,607.
    QuanserEncoder encoder;
    /// The Quanser Q8-USB supports eight quadrature encoder velcity inputs.
    /// Hence encoder channel numbers range from 0 to 7. The encoder velocity 
    /// measurements provide velocity information with no differentiation 
    /// required. The velocities generally have higher resolution  than those 
    /// obtained by differentiation, particularly at slow velocities,  and can
    /// provide instantaneous velocities.
    QuanserEncoderVelocity velocity;    
    /// Q8-USB watchdog timer. The timer may be programmed with any interval 
    /// between 125us upward. Set AO, DO, and PWM expire values you
    /// desire on those Modules. They will be set automatically through 
    /// hardware in the event of a software failure (i.e. watchdog timout).
    QuanserWatchdog watchdog;
};

} // namespace daq
} // namespace mahi