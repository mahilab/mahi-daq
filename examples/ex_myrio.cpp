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

int main(int argc, char** argv) {

    MahiLogger->set_max_severity(Verbose);

    /// For this example, connect MSPC AO[0] to AI[0]
    /// DIO[1] to DIO[3]
    /// Encoder to MSPC DIO[0,2]
    MyRio myrio;
    if (!myrio.is_open())
        return 1;

    /// MyRio is structured as 1 main DAQ and 3 nested DAQs
    /// for each connector interface. This example will only 
    /// focus on MSPC, but the other connectors are the same.
    print_info(myrio);
    print_info(myrio.mxpA);
    print_info(myrio.mxpB);
    print_info(myrio.mspC);
    

    /// By default, all DIO pins configured as DI channels,
    /// so we must enable DO channels. This will free DI[1].
    myrio.mspC.DO.set_channels({1});

    // Set enable values
    myrio.mspC.AO.enable_values[0] = 3.14;
    myrio.mspC.DO.enable_values[1] = TTL_HIGH;

    /// On myRIO, DIO pins are shared with encoders. We must
    /// enable the encoder channels we wish to use. This will
    /// free DI[0,2,4,6].
    myrio.mspC.encoder.set_channels({0,1});
    // Set the units on one of our encoders
    myrio.mspC.encoder.units[0] = 360.0 / 512;
    // Zero the encoder
    myrio.mspC.encoder.zero(0);

    /// Enable, this will set enable values on AO and DO, which we can read on AI[0] and DI[0]
    myrio.enable();
    sleep(1_ms);
    /// Can read the whole Module
    myrio.mspC.AI.read();
    /// Or just a single channel
    myrio.mspC.DI.read(3);
    print("AI[0]: {:+.2f} V", myrio.mspC.AI[0]);
    print("DI[0]: {}",(int)myrio.mspC.DI[3]);

    prompt("Press ENTER to start I/O loop.");

    /// General I/O loop
    Clock clk;
    while (!myrio.is_button_pressed()) {
        /// Synced read, reads all DAQ inputs
        myrio.read_all();
        print("AI[0]: {:+.2f} V | DI[3]: {} | encoder[0]: {} = {:+.2f} deg.", myrio.mspC.AI[0], (int)myrio.mspC.DI[3], myrio.mspC.encoder[0], myrio.mspC.encoder.positions[0]);
        double out = 5 * std::sin(TWOPI * clk.get_elapsed_time().as_seconds());
        myrio.mspC.AO[0] = out;
        myrio.mspC.DO[1] = out > 0 ? TTL_HIGH : TTL_LOW;
        /// Synced write, writes all DAQ outputs
        myrio.write_all();
        sleep(10_ms);
    }

    /// LED sin wave. The LED module is simply a DO module.
    for (int i = 0; i < 500; ++i) {
        double s = std::sin(TWOPI * i * 0.01);
        myrio.LED[0] = s < -0.5 ? TTL_HIGH : TTL_LOW;
        myrio.LED[1] = s > -0.5 && s < 0 ? TTL_HIGH : TTL_LOW;
        myrio.LED[2] = s > 0 && s < 0.5 ? TTL_HIGH : TTL_LOW;
        myrio.LED[3] = s > 0.5 ? TTL_HIGH : TTL_LOW;
        myrio.write_all();
        sleep(10_ms);
    }
    // turn off all LEDs
    myrio.LED.write({0,1,2,3},{TTL_LOW,TTL_LOW,TTL_LOW,TTL_LOW});

    /// Restore default channels. myRIO channels are persistant, meaning they will
    /// carry over to the next session. Here, we will manually reset all our DIs.
    myrio.mspC.DI.set_channels(myrio.mspC.DI.channels_allowed());

    /// myRIO will auto disable/close on destruction, but explicit calls are good practice
    myrio.disable();
    myrio.close();
 
}
