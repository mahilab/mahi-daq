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
    Q8Usb q8;
    if (!q8.is_open())
        return 1;

    /// Print the DAQ info
    print_info(q8);

    // Set enable values
    q8.AO.enable_values[0] = 3.14;
    q8.DO.enable_values[0] = HIGH;
    
    // Set the units on one of our encoders
    q8.encoder.units[0] = 360.0 / 512;
    // Zero the encoder
    q8.encoder.zero(0);

    /// Enable, this will set enable values on AO and DO, which we can read on AI[0] and DI[0]
    q8.enable();
    sleep(1_ms);
    /// Can read the whole Module
    q8.AI.read();
    /// Or just a single channel
    q8.DI.read(0);
    print("AI[0]: {:+.2f} V", q8.AI[0]);
    print("DI[0]: {}",(int)q8.DI[0]);

    prompt("Press ENTER to start I/O loop.");

    /// General I/O loop
    for (int i = 0; i < 500; ++i) {
        /// Synced read, reads all DAQ inputs
        q8.read_all();
        print("AI[0]: {:+.2f} V | DI[0]: {} | encoder[0]: {} = {:+.2f} deg.", q8.AI[0], (int)q8.DI[0], q8.encoder[0], q8.encoder.converted[0]);
        double out = 5 * std::sin(TWOPI * i * 0.01);
        q8.AO[0] = out;
        q8.DO[0] = out > 0 ? HIGH : LOW;
        /// Synced write, writes all DAQ outputs
        q8.write_all();
        sleep(10_ms);
    }

    // Enable PWM[1] (will disable DO[0])
    q8.PWM.set_channels({0});
    /// Set PWM output mode
    q8.PWM.modes.write(0, QuanserPwm::Mode::DutyCycle);
    /// Set PWM frequency
    q8.PWM.frequencies.write(0, 20000); // 20 MHz

    prompt("Press ENTER to start PWM output.");

    /// Read PWM out in to DO[0] (this loop is too slow to actually keep up, but we can see that it is changing)
    for (int i = 0; i < 500; ++i) {
        q8.read_all();
        print("DI[0]: {}", (int)q8.DI[0]);
        double duty_cycle = 0.5f + 0.5f * std::sin(TWOPI * i * 0.01);
        q8.PWM.write(0,duty_cycle);
        sleep(10_ms);
    }

    /// Restore default DO channels (disables PWM[0])
    q8.DO.set_channels({0,1,2,3,4,5,6,7});

    /// Q8Usb will auto disable/close on destruction, but explicit calls are good practice
    q8.disable();
    q8.close();

    return 0;
}
