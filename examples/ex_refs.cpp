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

class CurrentAmplifier {
public:
    CurrentAmplifier(double& command_voltage, double& sense_voltage, double gain) : 
        m_command_voltage(command_voltage), 
        m_sense_voltage(sense_voltage),
        m_gain(gain) 
    { }
    void command(double amps) {
        m_command_voltage = amps / m_gain;
    }
    double sense() {
        return m_gain * m_sense_voltage;
    }
    double& m_command_voltage;
    double& m_sense_voltage;
    double m_gain; // A/V
};

int main(int argc, char const *argv[])
{
    // create a VirtualDaq for example, but same applies to real DAQs
    VirtualDaq daq("my_virtual_daq");

    // set up virtual sinks/sources so we can see what would happen
    daq.AI.sources[0] = []()          { print("AI Read:  2 V"); return 2; };
    daq.AO.sinks[0]   = [](Voltage v) { print("AO Write: {} V", v);       };

    // can pass reference to the buffer from the Module object or a Channel object
    CurrentAmplifier amp(daq.AO[0], daq.AI.channel(0).get(), 0.1);

    // update the DAQ inputs
    daq.AI.update();

    // get the current sense from the amplifier
    print("Sense:    {} A", amp.sense());
    // set the amplifier command current
    amp.command(0.25);
    print("Command:  {} A", 0.25);

    // update the DAQ outputs
    daq.AO.update();

    return 0;
}
