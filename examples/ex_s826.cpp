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

using namespace mahi::daq;
using namespace mahi::util;

ctrl_bool g_stop_flag(false);

bool my_handler(CtrlEvent event)
{
    if (event == CtrlEvent::CtrlC)
        g_stop_flag = true;
    return true;
}

int main(int argc, char const *argv[])
{
    register_ctrl_handler(my_handler);

    S826 s826;
    s826.open();
    s826.enable();
    // s826.AI.set_settling_time(microseconds(3));
    s826.encoder.channel(0).zero();

    Timer timer(1000_Hz);
    timer.disable_warnings();
    Time t;

    Waveform sinewave(Waveform::Sin, seconds(1));

    //  s826.encoder[0].reset_count(0);
     s826.encoder.channel(0).set_units_per_count(360.0 / 512);

     std::vector<double> buffer(5);

     Differentiator diff1;
     Butterworth filt(2, 100_Hz, 1000_Hz);
    //  Differentiator diff2;

    while (!g_stop_flag)
    {
        s826.AI.update();
        s826.encoder.update();
        buffer[0] = sinewave(t);
        buffer[1] = s826.AI[0];
        buffer[2] = s826.encoder.channel(0).get_position();
        buffer[3] = s826.encoder.channel(0).get_velocity();
        double vel = filt.update(buffer[3]); 
        buffer[4] = vel;
        s826.AO[0] = buffer[0];
        s826.AO.update();
        t = timer.wait();
    }

    print("Wait Ratio:", timer.get_wait_ratio());
    print("Misses:", timer.get_misses());

    return 0;
}
