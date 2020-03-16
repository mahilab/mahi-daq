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

using namespace mahi::util;
using namespace mahi::daq;

int cpu_test(int frequency, int time) {
    int iterations = frequency * time;
    Timer timer(hertz(frequency), Timer::Busy, false);
    Time t = Time::Zero;
    LOG(Info) << "---Starting Test---";
    LOG(Info) << "Target Frequency: " << frequency << " Hz";
    LOG(Info) << "Target Period:    " << timer.get_period().as_microseconds() << " us";
    LOG(Info) << "Target Time:      " << time << " s (" << time * 1000000 << " us)";
    LOG(Info) << "Iterations:       " << iterations;
    timer.restart();
    for (int i = 0; i < iterations; ++i)
        t = timer.wait();
    LOG(Info) << "---Test Completed---";
    LOG(Info) << "Actual Frequency: " << iterations / t.as_seconds() << " Hz";
    LOG(Info) << "Actual Period:    " << (double)t.as_microseconds() / (double)iterations << " us";
    LOG(Info) << "Actual Time:      " << t.as_seconds() << " s (" << t.as_microseconds() << " us)";
    LOG(Info) << "Timer Misses:     " << timer.get_misses();
    LOG(Info) << "Timer Miss Rate:  " << timer.get_miss_rate();
    LOG(Info) << "Timer Wait Ratio: " << timer.get_wait_ratio();
    return 0;
}

template <typename TDaq>
int analog_test(TDaq& daq, int frequency, int time, int ai, int ao, bool save) {

    int iterations = frequency * time;
    std::vector<std::array<double,3>> data(iterations);
    Waveform sinwave(Waveform::Sin, hertz(100), 10);
    Timer timer(hertz(frequency), Timer::Busy, false);
    Time t = Time::Zero;

    LOG(Info) << "---Starting Test---";
    LOG(Info) << "DAQ:              " << daq.name();
    LOG(Info) << "DAQ AO Module:    " << daq.AO.name();
    LOG(Info) << "Target Frequency: " << frequency << " Hz";
    LOG(Info) << "Target Period:    " << timer.get_period().as_microseconds() << " us";
    LOG(Info) << "Target Time:      " << time << " s (" << time * 1000000 << " us)";
    LOG(Info) << "Iterations:       " << iterations;
    LOG(Info) << "AI/AO Channels:   " << ai << "/" << ao;

    timer.restart();
    for (int i = 0; i < iterations; ++i) {
        daq.AI.read();
        data[i][0] = t.as_seconds();
        data[i][1] = daq.AI[ai];
        data[i][2] = sinwave(t);
        daq.AO[ao] = data[i][2];
        daq.AO.write();
        t = timer.wait();
    }

    int repeats = 0;
    for (int i = 1; i < iterations; ++i) {
        if (data[i][1] == data[i-1][1])
            repeats++;
    }

    LOG(Info) << "---Test Completed---";
    LOG(Info) << "Repeats:          " << repeats;
    LOG(Info) << "Repeat Percent:   " << 100 * (double)repeats / (double)(iterations - 1) << "%";
    LOG(Info) << "Actual Frequency: " << iterations / t.as_seconds() << " Hz";
    LOG(Info) << "Actual Period:    " << (double)t.as_microseconds() / (double)iterations << " us";
    LOG(Info) << "Actual Time:      " << t.as_seconds() << " s (" << t.as_microseconds() << " us)";
    LOG(Info) << "Timer Misses:     " << timer.get_misses();
    LOG(Info) << "Timer Miss Rate:  " << timer.get_miss_rate();
    LOG(Info) << "Timer Wait Ratio: " << timer.get_wait_ratio();

    if (save) {
        LOG(Info) << "Saving Test Data ...";
        Timestamp stamp;
        std::vector<std::string> header = {"Time", "AI_" + std::to_string(ai), "AO_" + std::to_string(ao)};
        std::string filename = daq.name() + "_f" + std::to_string(frequency) + "_t" + std::to_string(time) + "_" + stamp.hh_mm_ss_mmm() + ".csv";
        csv_write_rows(filename, header);
        csv_append_rows(filename, data);
    }

    return 0;
}

int main(int argc, char* argv[]) {

    if (MahiLogger)
        MahiLogger->set_max_severity(Info);

    Options options("perf.exe", "Utility Program to Evaluate DAQ Analog Loopback Performance");
    options.add_options()
        ("d", "The type of DAQ to test (q2, q8, qpid, s826, myrio, or cpu).", value<std::string>())
        ("f", "The target frequency in Hz (default = 1000).",                 value<int>())
        ("t", "The test duration in seconds (default = 10).",                 value<int>())
        ("i", "Input channel for loopback (default = 0)",                     value<int>())
        ("o", "Output channel for loopback (default = 0)",                    value<int>())
        ("r", "Enable Windows realtime thread priority.")
        ("s", "Save test data.")
        ("h", "Prints helpful information.");
    auto user_input = options.parse(argc, argv);    
    if (user_input.count("h") > 0) { 
        print("{}",options.help());
        return 0;
    }
    int frequency = 1000;
    if (user_input.count("f"))
        frequency = user_input["f"].as<int>();
    int time  = 10;
    if (user_input.count("t"))
        time = user_input["t"].as<int>();
    int ai = 0;
    if (user_input.count("i"))
        ai = user_input["i"].as<int>();
    int ao = 0;
    if (user_input.count("o"))
        ao = user_input["o"].as<int>();

    if (user_input.count("r")) {
        if (enable_realtime()) {
            LOG(Info) << "Enabled Windows Realtime thread priority";
        }
    }

    bool save = user_input.count("s") > 0;

    if (user_input.count("d")) {
        auto str = user_input["d"].as<std::string>();
        if (str == "cpu") {
            return cpu_test(frequency, time);
        }        
#ifdef MAHI_QUANSER
        else if (str == "q2") {
            Q2Usb q2;
            q2.enable();
            return analog_test(q2, frequency, time, ai, ao, save);
        }
        else if (str == "q8") {
            Q8Usb q8;
            q8.enable();
            return analog_test(q8, frequency, time, ai, ao, save);
        }
        else if (str == "qpid") {
            QPid qpid;
            qpid.enable();
            return analog_test(qpid, frequency, time, ai, ao, save);
        }
#endif
#ifdef MAHI_SENSORAY
        else if (str == "s826") {
            S826 s826;
            s826.enable();
            return analog_test(s826, frequency, time, ai, ao, save);
        }
#endif
#ifdef MAHI_MYRIO
        else if (str == "myrio-c") {
            MyRio myrio;
            myrio.enable();
            return analog_test(myrio.mspC, frequency, time, ai, ao, save);
        }
#endif
        else {
            LOG(Error) << "-d " << str << " is not a valid option";
            print("{}",options.help());
            return 1;
        }
    }
    else {
        LOG(Error) << "No -d option specified";
        print("{}",options.help());
        return 1;
    }
}
