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

ctrl_bool stop(false);

int main(int argc, char const *argv[])
{
    MahiLogger->set_max_severity(Verbose);

    // For this example, connect AO0 to AI0 and DIO0 to DIO1, 
    // and connect an encoder to channel 0.
    Q2Usb q2;
    if (!q2.is_open())
        return 1;

    /// Print the DAQ info
    print_info(q2);

    // Q2-USB DI and DO share pins. All pins default to inputs, 
    /// so set the pins you wish to use as DO
    q2.DO.set_channels({1,8}); // DI[1] will be disabled, DO[8] is the LED

    // Set enable values
    q2.AO.enable_values[0] = 3.14;
    q2.DO.enable_values[1] = HIGH;
    
    // Set the units on one of our encoders
    q2.encoder.units[0] = 360.0 / 512;
    // Zero the encoder
    q2.encoder.zero(0);

    /// Enable, this will set enable values on AO and DO, which we can read on AI[0] and DI[0]
    q2.enable();
    sleep(1_ms);
    /// Can read the whole Module
    q2.AI.read();
    /// Or just a single channel
    q2.DI.read(0);
    print("AI[0]: {:+.2f} V", q2.AI[0]);
    print("DI[0]: {}",(int)q2.DI[0]);

    prompt("Press ENTER to start I/O loop.");

    /// General I/O loop
    for (int i = 0; i < 500; ++i) {
        /// Synced read, reads all DAQ inputs
        q2.read_all();
        print("AI[0]: {:+.2f} V | DI[0]: {} | encoder[0]: {} = {:+.2f} deg.", q2.AI[0], (int)q2.DI[0], q2.encoder[0], q2.encoder.converted[0]);
        double out = 5 * std::sin(TWOPI * i * 0.01);
        q2.AO[0] = out;
        q2.DO[1] = out > 0 ? HIGH : LOW;
        q2.DO[8] = q2.DO[1]; // blink LED
        /// Synced write, writes all DAQ outputs
        q2.write_all();
        sleep(10_ms);
    }

    // Enable PWM[1] (will disable DO[1])
    q2.PWM.set_channels({1});
    /// Set PWM output mode
    q2.PWM.modes.write(1, QuanserPwm::Mode::DutyCycle);
    /// Set PWM frequency
    q2.PWM.frequencies.write(1, 20000); // 20 MHz

    prompt("Press ENTER to start PWM output.");

    /// Read PWM out in to DO[0] (this loop is too slow to actually keep up, but we can see that it is changing)
    for (int i = 0; i < 500; ++i) {
        q2.read_all();
        print("DI[0]: {}", (int)q2.DI[0]);
        double duty_cycle = 0.5f + 0.5f * std::sin(TWOPI * i * 0.01);
        q2.PWM.write(1,duty_cycle);
        sleep(10_ms);
    }

    /// Restore default DI channels (disables PWM[1])
    q2.DI.set_channels({0,1,2,3,4,5,6,7});

    /// Q2Usb will auto disable/close on destruction, but explicit calls are good practice
    q2.disable();
    q2.close();

    return 0;
}
