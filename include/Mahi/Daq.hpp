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

// #include <Mahi/Daq/ChannelBase.hpp>
#include <Mahi/Daq/Daq.hpp>
// #include <Mahi/Daq/Encoder.hpp>
#include <Mahi/Daq/Io.hpp>
// #include <Mahi/Daqdaqba/InputOutput.hpp>
// #include <Mahi/Daq/Module.hpp>
// #include <Mahi/Daq/Output.hpp>
// #include <Mahi/Daq/Types.hpp>
// #include <Mahi/Daq/VirtualDaq.hpp>
#include <Mahi/Daq/Watchdog.hpp>

#ifdef MAHI_QUANSER
    // #include <Mahi/Daq/Quanser/Q2Usb.hpp>
    // #include <Mahi/Daq/Quanser/Q8Usb.hpp>
    // #include <Mahi/Daq/Quanser/QPid.hpp>
    // #include <Mahi/Daq/Quanser/QuanserAI.hpp>
    // #include <Mahi/Daq/Quanser/QuanserAO.hpp>
    // #include <Mahi/Daq/Quanser/QuanserDaq.hpp>
    // #include <Mahi/Daq/Quanser/QuanserDI.hpp>
    // #include <Mahi/Daq/Quanser/QuanserDIO.hpp>
    // #include <Mahi/Daq/Quanser/QuanserDO.hpp>
    // #include <Mahi/Daq/Quanser/QuanserEncoder.hpp>
    // #include <Mahi/Daq/Quanser/QuanserOptions.hpp>
    // #include <Mahi/Daq/Quanser/QuanserPwm.hpp>
    // #include <Mahi/Daq/Quanser/QuanserWatchdog.hpp>
    #include <Mahi/Daq/Quanser/Q8Usb.hpp>
    #include <Mahi/Daq/Quanser/Q2Usb.hpp>
#endif

#ifdef MAHI_SENSORAY
    #include <Mahi/Daq/Sensoray/S826.hpp>
    #include <Mahi/Daq/Sensoray/S826AI.hpp>
    #include <Mahi/Daq/Sensoray/S826AO.hpp>
    #include <Mahi/Daq/Sensoray/S826DIO.hpp>
    #include <Mahi/Daq/Sensoray/S826Encoder.hpp>
    #include <Mahi/Daq/Sensoray/S826Watchdog.hpp>
#endif

#ifdef MAHI_MYRIO
    #include <Mahi/Daq/NI/MyRio/MyRio.hpp>
    #include <Mahi/Daq/NI/MyRio/MyRioAI.hpp>
    #include <Mahi/Daq/NI/MyRio/MyRioAO.hpp>
    #include <Mahi/Daq/NI/MyRio/MyRioConnector.hpp>
    #include <Mahi/Daq/NI/MyRio/MyRioDIO.hpp>
    #include <Mahi/Daq/NI/MyRio/MyRioEncoder.hpp>
    #include <Mahi/Daq/NI/MyRio/MyRioI2C.hpp>
#endif