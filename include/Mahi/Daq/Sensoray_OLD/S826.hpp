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
// #include <Mahi/Daq/Sensoray/S826AI.hpp>
// #include <Mahi/Daq/Sensoray/S826AO.hpp>
// #include <Mahi/Daq/Sensoray/S826DIO.hpp>
// #include <Mahi/Daq/Sensoray/S826Encoder.hpp>
// #include <Mahi/Daq/Sensoray/S826Watchdog.hpp>
#include <Mahi/Util/Timing/Time.hpp>

namespace mahi {
namespace daq {

class S826 : public Daq {
public:
    /// Constructor 
    S826(int board = 0);
    /// Destructor
    ~S826();
    /// Returns the current board time
    util::Time time() const;
public:
    // ...
private:    
    virtual bool on_daq_open() override;
    virtual bool on_daq_close() override;
    virtual bool on_daq_enable() override;
    virtual bool on_daq_disable() override;
private:
    /// S826 board identification number
    int m_board; 
};

} // namespace daq
} // namespace mahi