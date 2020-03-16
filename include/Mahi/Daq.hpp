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

#include <Mahi/Daq/Daq.hpp>
#include <Mahi/Daq/Module.hpp>
#include <Mahi/Daq/Buffer.hpp>
#include <Mahi/Daq/Io.hpp>
#include <Mahi/Daq/Watchdog.hpp>
#include <Mahi/Daq/Utils.hpp>

#ifdef MAHI_QUANSER
    #include <Mahi/Daq/Quanser/Q2Usb.hpp>
    #include <Mahi/Daq/Quanser/Q8Usb.hpp>
    #include <Mahi/Daq/Quanser/QPid.hpp>
#endif

#ifdef MAHI_SENSORAY
    #include <Mahi/Daq/Sensoray/S826.hpp>
#endif

#ifdef MAHI_MYRIO
    #include <Mahi/Daq/NI/MyRio/MyRio.hpp>
#endif