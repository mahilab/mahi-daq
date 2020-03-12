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

bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        stop = true;
    return true;
}

int main(int argc, char const *argv[])
{
    MahiLogger->set_max_severity(Verbose);

    // For this example, connect AO0 to AI0 and DIO0 to DIO1, 
    // and connect an encoder to channel 0.
    Q2Usb q2;

    /// Print the module names
    for (auto& m : q2.modules())
        print("{}", m->name());

    // Q2-USB DI and DO share pins. All pins default to inputs, 
    /// so set the pins you wish to use as DO
    q2.DO.set_channels({1,8}); // note: 8 is the LED

    // set enable values
    q2.AO.enable_values[0] = 1.0;
    q2.DO.enable_values[1] = HIGH;
    
    // set the units on one of our encoders
    q2.encoder.units[0] = 360.0 / 512;
    // zero the encoder
    q2.encoder.zero(0);

    /// enable, this will set enable values on AO and DO
    q2.enable();
    sleep(1_ms);
    q2.AI.read();
    q2.DI.read();
    print("AI[0]: {:+.2f} V", q2.AI[0]);
    print("DI[0]: {}",(int)q2.DI[0]);

    prompt("Press ENTER to start I/O loop");

    /// Loop back and encoder read
    for (int i = 0; i < 500; ++i) {
        q2.read_all();
        print("AI[0]: {:+.2f} V | DI[0]: {} | encoder[0]: {} = {:+.2f} deg.", q2.AI[0], (int)q2.DI[0], q2.encoder[0], q2.encoder.converted[0]);
        double out = 5 * std::sin(TWOPI * i * 0.01);
        q2.AO[0] = out;
        q2.DO[1] = out > 0 ? HIGH : LOW;
        q2.DO[8] = q2.DO[1]; // blink LED
        q2.write_all();
        sleep(10_ms);
    }

    prompt("Press ENTER to start PWM loop");

    // enable PWM
    q2.PWM.set_channels({0,1});
    q2.PWM.frequencies.write(0, 20000);

    /// Loop back and encoder read
    for (int i = 0; i < 500; ++i) {
        q2.read_all();
        double out = 0.5f + 0.5f * std::sin(TWOPI * i * 0.01);
        q2.PWM.write(0,out);
        sleep(10_ms);
    }


    return 0;
}
