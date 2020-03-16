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

/// A simple little Current Amplifier class that can command and sense amps.
class CurrentAmplifier {
public:
    CurrentAmplifier(double& command_voltage, const double& sense_voltage, double gain) : 
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
    const double& m_sense_voltage;
    double m_gain; // A/V
};

/// A simple structured environment for doing controls
template <class TDaq>
class ControlLoop {
public:
    void start() {
        running = true;
        Timer timer(10000_Hz);
        Time t;
        while (running) {
            daq.read_all();
            update(t);
            daq.write_all();
            t = timer.wait();
        }
        print("Wait Ratio: {}",timer.get_wait_ratio());
        print("Misses:     {}",timer.get_misses());

    }
protected:
    void stop() { running = false; }
    virtual void update(Time t) { };
    TDaq daq;
private:
    bool running;
};

/// Our custom implementation of ControLoop
template <typename TDaq>
class MyControlLoop : public ControlLoop<TDaq> {
public:
    MyControlLoop() : amp(this->daq.AO[0], this->daq.AI[0], 0.5) { }
private:
    void update(Time t) override {
        double amps_sense  = amp.sense();
        double amps_command = std::sin(TWOPI * 10 * t.as_seconds());
        amp.command(amps_command);
        if (t > 5_s)
            this->stop();
    }
    CurrentAmplifier amp;
};

int main(int argc, char const *argv[])
{
    /// Template this on whatever DAQ you want
    MyControlLoop<Q2Usb> loop;
    loop.start();
    return 0;
}
