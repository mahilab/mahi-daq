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

// See Handles.hpp

// Buffered gets/sets is usually the most efficient way to use Handles 
// if you are sampling a large number of channels.
void handle_example_buffered(AIHandle ai, AOHandle ao, EncoderHandle enc) {
    static Clock clk;
    fmt::print("AI[0]: {:+.2f} V | encoder[0]: {} = {:+.2f} deg.", ai.get_volts(), enc.get_counts(), enc.get_pos());
    Volts out = 5 * std::sin(TWOPI * clk.get_elapsed_time().as_seconds());
    ao.set_volts(out);
}

// Immediate read/writes may less efficient for large numbers of channels, 
// but more efficient for smaller numbers. Perform your own benchmarks.
void handle_example_immediate(AIHandle ai, AOHandle ao, EncoderHandle enc) {
    static Clock clk;
    fmt::print("AI[0]: {:+.2f} V | encoder[0]: {} = {:+.2f} deg.", ai.read_volts(), enc.read_counts(), enc.read_pos());
    Volts out = 5 * std::sin(TWOPI * clk.get_elapsed_time().as_seconds());
    ao.write_volts(out);
}

int main(int argc, char const *argv[])
{
#ifdef MAHI_QUANSER
    Q8Usb daq;
    daq.enable();
#elif MAHI_SENSORAY
    S826 daq;
    daq.enable();
#elif MAHI_MYRIO
    MyRio myrio;
    myrio.enable();
    myrio.mspC.encoder.set_channels({0});
    auto& daq = myrio.mspC;
    
#endif

    auto ai  = AIHandle(daq.AI,0);
    auto ao  = AOHandle(daq.AO,0);
    auto enc = EncoderHandle(daq.encoder,0); 


    prompt("Press Enter to Start Buffered I/O");
    for (int i = 0; i < 1000; ++i) {
        Clock clk;
        daq.read_all();
        handle_example_buffered(ai,ao,enc);
        daq.write_all();
        fmt::print(" ({})\n", clk.get_elapsed_time());
        sleep(10_ms);
    }

    prompt("Press Enter to Start Immediate I/O");
    for (int i = 0; i < 1000; ++i) {
        Clock clk;
        handle_example_immediate(ai,ao,enc);
        fmt::print(" ({})\n", clk.get_elapsed_time());
        sleep(10_ms);
    }

    return 0;
}
