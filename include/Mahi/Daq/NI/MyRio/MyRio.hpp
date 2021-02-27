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
#include <Mahi/Daq/Io.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioConnector.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioLed.hpp>

namespace mahi {
namespace daq {

/// National Instruments myRIO embedded system
class MyRio : public Daq {
public:
    /// Constructor
    MyRio();
    /// Default Destructor
    ~MyRio();
    /// Resets the myRIO FPGA to default configuration
    // bool reset();
    /// Updates all connector inputs simultaneously.
    bool read_all() override;
    /// Updates all connector outputs simultaneously.
    bool write_all() override;
    /// Returns true if the myRIO button is currently pressed
    bool is_button_pressed() const;
    /// Prints configuration and status registers
    void print_registers() const;
public:
    MyRioMxp mxpA;  ///< MXP connector A
    MyRioMxp mxpB;  ///< MXP connector B
    MyRioMsp mspC;  ///< MSP connector C
    MyRioLed LED;   ///< LEDs
private:
    bool on_daq_open() override;
    bool on_daq_close() override;
    bool on_daq_enable() override;
    bool on_daq_disable() override;
};

}  // namespace daq
}  // namespace mahi
