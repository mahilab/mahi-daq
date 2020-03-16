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

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);

bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        stop = true;
    return true;
}

int main() {

    // register CTRL-C handler
    register_ctrl_handler(handler);

    //===============================================================================
    // CONSTUCT/OPEN/CONFIGURE
    //===============================================================================

    // create default Q8 USB object
    // (all channels enabled, auto open on, sanity check on)
    QPid qpid;
    if (!qpid.open())
        return 1;


    //===============================================================================
    // ENABLE
    //===============================================================================

    // enable qpid USB
    if (!qpid.enable())
        return 1;

    // qpid.AO[0].set(5.0);
    // qpid.update_output();
    // qpid.update_input();
    // double in = qpid.AI[0].get();
    // print(in);

    

    qpid.DIO.channel(0).set_direction(Direction::Out);
    qpid.DIO.channel(1).set_direction(Direction::Out);
    qpid.DIO.channel(8).set_direction(Direction::In);
    qpid.DIO.channel(9).set_direction(Direction::In);

    print_var(qpid.DIO.output_channel_numbers());
    print_var(qpid.DIO.input_channel_numbers());

    qpid.DIO[0] = Logic::High;
    qpid.DIO[1] = Logic::Low;

    print_var(qpid.DIO.get(0));
    print_var(qpid.DIO.get(1));

    qpid.update_output();

    prompt("Press ENTER to Measure");

    sleep(seconds(1));

    qpid.update_input();
    double input = qpid.DIO[8];
    print_var(input);
    input = qpid.DIO[9];
    print_var(input);

    // qpid.watchdog.start();

    // for (int i = 0; i < 1000; ++i) {
    //     qpid.update_input();
    //     print(qpid.encoder[0].get());
    //     sleep(milliseconds(1));
    // }

    sleep(seconds(1));

    // qpid.watchdog.stop();
    

    // disable qpid USB
    qpid.disable();
    // close qpid USB
    qpid.close();

    return 0;
}
