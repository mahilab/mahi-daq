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

#include <mahi/daq/Buffer.hpp>
#include <mahi/daq/ChannelBase.hpp>
#include <mahi/daq/DaqBase.hpp>
#include <mahi/daq/Device.hpp>
#include <mahi/daq/Encoder.hpp>
#include <mahi/daq/Input.hpp>
#include <mahi/daq/InputOutput.hpp>
#include <mahi/daq/Module.hpp>
#include <mahi/daq/Output.hpp>
#include <mahi/daq/Types.hpp>
#include <mahi/daq/VirtualDaq.hpp>
#include <mahi/daq/Watchdog.hpp>

#ifdef MAHI_QUANSER
    #include <mahi/daq/Quanser/Q2Usb.hpp>
    #include <mahi/daq/Quanser/Q8Usb.hpp>
    #include <mahi/daq/Quanser/QPid.hpp>
    #include <mahi/daq/Quanser/QuanserAI.hpp>
    #include <mahi/daq/Quanser/QuanserAO.hpp>
    #include <mahi/daq/Quanser/QuanserDaq.hpp>
    #include <mahi/daq/Quanser/QuanserDI.hpp>
    #include <mahi/daq/Quanser/QuanserDIO.hpp>
    #include <mahi/daq/Quanser/QuanserDO.hpp>
    #include <mahi/daq/Quanser/QuanserEncoder.hpp>
    #include <mahi/daq/Quanser/QuanserOptions.hpp>
    #include <mahi/daq/Quanser/QuanserPwm.hpp>
    #include <mahi/daq/Quanser/QuanserWatchdog.hpp>
#endif

#ifdef MAHI_SENSORAY
    #include <mahi/daq/Sensoray/S826.hpp>
    #include <mahi/daq/Sensoray/S826AI.hpp>
    #include <mahi/daq/Sensoray/S826AO.hpp>
    #include <mahi/daq/Sensoray/S826DIO.hpp>
    #include <mahi/daq/Sensoray/S826Encoder.hpp>
    #include <mahi/daq/Sensoray/S826Watchdog.hpp>
#endif

#ifdef MAHI_MYRIO
    #include <mahi/daq/NI/MyRio/MyRio.hpp>
    #include <mahi/daq/NI/MyRio/MyRioAI.hpp>
    #include <mahi/daq/NI/MyRio/MyRioAO.hpp>
    #include <mahi/daq/NI/MyRio/MyRioConnector.hpp>
    #include <mahi/daq/NI/MyRio/MyRioDIO.hpp>
    #include <mahi/daq/NI/MyRio/MyRioEncoder.hpp>
    #include <mahi/daq/NI/MyRio/MyRioI2C.hpp>
#endif