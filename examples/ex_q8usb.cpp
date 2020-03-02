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

#include <mahi/daq.hpp>
#include <iostream>
#include <thread>

using namespace mahi::daq;

int main() {

    //==============================================================================
    // CONSTUCT/OPEN/CONFIGURE
    //==============================================================================

    // create default Q8 USB object (all channels enabled, auto open on, sanity check on)
    Q8Usb q8;
    if (!q8.open())
        return 1;

    // override default enable/disable/expiration states
    q8.DO.set_enable_values(std::vector<Logic>(8, High));  // default is LOW
    q8.DO.set_disable_values(std::vector<Logic>(8, High)); // default is LOW
    q8.DO.set_expire_values(std::vector<Logic>(8, High));  // default is LOW
    q8.AO.set_enable_values(std::vector<Voltage>(8, 1.0));  // default is 0.0
    q8.AO.set_disable_values(std::vector<Voltage>(8, 2.0)); // default is 0.0
    q8.AO.set_expire_values(std::vector<Voltage>(8, 3.0));  // default is 0.0

    //==============================================================================
    // ENABLE
    //==============================================================================

    std::cout << "Press any key to enable Q8 USB." << std::endl;
    getchar();
    if (!q8.enable())
        return 1;

    //==============================================================================
    // ENCODER
    //==============================================================================

    std::cout << "Connect an encoder to channel 0 then press any key to Encoder start test." << std::endl;
    getchar();
    // sample encoder 
    for (int i = 0; i < 500; ++i) {
        q8.update_input();
        std::cout << q8.encoder[0].get_value() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    //==============================================================================
    // ANALOG INPUT/OUTPUT
    //==============================================================================

    std::cout << "Connect AO0 to AI0 then press any key to start AIO test." << std::endl;
    getchar();
    // analog loopback loop
    double t = 0;
    for (int i = 0; i < 500; ++i) {
        q8.update_input();
        std::cout << q8.AI.get_value(0) << std::endl;
        double voltage = std::sin(t);
        q8.AO.set_value(0, voltage);
        q8.update_output();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        t += 0.010;
    }

    //==============================================================================
    // DIGITAL INPUT/OUTPUT
    //==============================================================================

    std::cout << "Connect DI0 to DO0 then press any key to start DIO test." << std::endl;
    getchar();
    Logic signal = High;
    // digital loopback
    for (int i = 0; i < 10; ++i) {
        q8.update_input();
        std::cout << q8.DI.get_value(0) << std::endl;
        signal = (Logic)(High - signal);
        q8.DO.set_value(0, signal);
        q8.update_output();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    //==============================================================================
    // WATCHDOG
    //==============================================================================

    // ask for user input
    std::cout << "Press any key to start the watchdog test" << std::endl;
    getchar();
    // set watchdog timeout (default value is 0.1 (100 ms))
    q8.watchdog.set_timeout(0.01);
    // start watchdog
    q8.watchdog.start();
    for (int i = 0; i < 5000; ++i) {
        // simulate a missed deadline
        if (i == 250) 
            std::this_thread::sleep_for(std::chrono::milliseconds(12));
        // kick the watchdog so it doesn't expire and check if it's still watching
        if (!q8.watchdog.kick())
            break;  // watchdog timeout out
        // wait the control loop timer
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    // check the state of the watchdog
    if (q8.watchdog.is_expired()) {
        std::cout << "Watchdog did expire. Stopping and clearing it." << std::endl;
        q8.watchdog.stop();
        q8.watchdog.clear();
    } else {
        std::cout << "Watchdog did not expire. Stopping it." << std::endl;
        q8.watchdog.stop();
    }

    //==============================================================================
    // DISABLE
    //==============================================================================

    std::cout << "Press any key to disable the Q8 USB." << std::endl;
    // disable Q8 USB
    q8.disable();
    // close Q8 USB
    q8.close();

    return 0;
}
