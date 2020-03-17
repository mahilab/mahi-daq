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
/// See mahi-robo for more classes like this!
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
private:
    double& m_command_voltage;
    const double& m_sense_voltage;
    double m_gain; // A/V
};

/// A simple structured environment for doing controls
class ControlLoop {
public:
    ControlLoop(Daq& daq) : daq(daq) { }
    void start() {
        daq.enable();
        running = true;
        Timer timer(100_Hz);
        Time t;
        while (running) {
            daq.read_all();
            update(t);
            daq.write_all();
            t = timer.wait();
        }
        daq.disable();
        daq.close();
    }
protected:
    void stop() { running = false; }
    virtual void update(Time t) { };
    Daq& daq;
private:
    bool running;
};

/// Our custom implementation of ControLoop
class MyControlLoop : public ControlLoop {
public:
    template <typename TDaq>
    MyControlLoop(TDaq& daq) : 
        ControlLoop(daq), 
        amp(daq.AO[0], daq.AI[0], 0.5) 
    { }
private:
    void update(Time t) override {
        double amps_sense  = amp.sense();
        double amps_command = std::sin(TWOPI * 0.25 * t.as_seconds());
        amp.command(amps_command);
        cls();
        print("Command: {:+.2f} , Sense: {:+.2f}", amps_command, amps_sense);
        if (t > 5_s)
            this->stop();
    }
    CurrentAmplifier amp;
};

int main(int argc, char const *argv[])
{
#ifdef MAHI_QUANSER
    Q8Usb q8;
    MyControlLoop loop(q8);
    loop.start();
#elif MAHI_SENSORAY
    S826 s826;
    MyControlLoop loop(s826);
    loop.start();
#elif MAHI_MYRIO
    MyRio myrio;
    MyControlLoop loop(myrio.mspC);
    loop.start();
#endif
    return 0;
}
