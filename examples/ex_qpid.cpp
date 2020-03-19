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

#include <Mahi/Daq.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::daq;
using namespace mahi::util;

int main(int argc, char const *argv[])
{
    MahiLogger->set_max_severity(Verbose);

    // For this example, connect AO0 to AI0 and DO0 to DI0, 
    // and connect an encoder to channel 0.
    QPid qpid;
    if (!qpid.is_open())
        return 1;

    /// Print the DAQ info
    print_info(qpid);

    // Set enable values
    qpid.AO.enable_values[0] = 3.14;
    qpid.DO.enable_values[0] = TTL_HIGH;
    
    // Set the units on one of our encoders
    qpid.encoder.units[0] = 360.0 / 512;
    // Zero the encoder
    qpid.encoder.zero(0);

    /// Enable, this will set enable values on AO and DO, which we can read on AI[0] and DI[0]
    qpid.enable();
    sleep(1_ms);
    /// Can read the whole Module
    qpid.AI.read();
    /// Or just a single channel
    qpid.DI.read(0);
    print("AI[0]: {:+.2f} V", qpid.AI[0]);
    print("DI[0]: {}",(int)qpid.DI[0]);

    prompt("Press ENTER to start I/O loop.");

    /// General I/O loop
    for (int i = 0; i < 500; ++i) {
        /// Synced read, reads all DAQ inputs
        qpid.read_all();
        print("AI[0]: {:+.2f} V | DI[0]: {} | encoder[0]: {} = {:+.2f} deg.", qpid.AI[0], (int)qpid.DI[0], qpid.encoder[0], qpid.encoder.positions[0]);
        double out = 5 * std::sin(TWOPI * i * 0.01);
        qpid.AO[0] = out;
        qpid.DO[0] = out > 0 ? TTL_HIGH : TTL_LOW;
        /// Synced write, writes all DAQ outputs
        qpid.write_all();
        sleep(10_ms);
    }

    /// Set PWM output mode
    qpid.PWM.modes.write(0, QuanserPwm::Mode::DutyCycle);
    /// Set PWM frequency
    qpid.PWM.frequencies.write(0, 20000); // 20 MHz

    prompt("Press ENTER to start PWM output.");

    /// Read PWM out in to DO[0] (this loop is too slow to actually keep up, but we can see that it is changing)
    for (int i = 0; i < 500; ++i) {
        qpid.read_all();
        print("DI[0]: {}", (int)qpid.DI[0]);
        double duty_cycle = 0.5f + 0.5f * std::sin(TWOPI * i * 0.01);
        qpid.PWM.write(0,duty_cycle);
        sleep(10_ms);
    }

    /// Q8Usb will auto disable/close on destruction, but explicit calls are good practice
    qpid.disable();
    qpid.close();

    return 0;
}
