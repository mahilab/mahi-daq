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

// This examples demonstrate how to use mahi::daq to create an interface for a
// DAQ not already implemented by the library. For more advanced examples, take
// a look at the already existing DAQ classes. If you do end up creating a new
// interface, please consider submitting a pull request!

// Since this will be implemented in a single file, we need to make a few
// forward declarations. Normally, you wouldn't do this because you would
// have split your Modules and Daq into separate header files.

class MyDaq;

/// DAQs are composed of Modules. A Module exposes one functionality of the DAQ
/// (e.g. analog output, digital input, encoders, PWM, watchdog, etc.). Here,
/// we will make an analog output Module, by subclassing AOModule from Io.hpp.
class MyAOModule : public AOModule {
public:
    MyAOModule(Daq& d, const ChanNums& allowed) : AOModule(d, allowed), ranges(*this, {-10, 10}) {
        // First, give this Module a name:
        set_name(d.name() + ".AO");

        // Wrap your DAQ's AO writing API into a lambda callback
        auto write_impl = [this](const ChanNum* chs, const Volts* volts, std::size_t n) {
            // This is where you implement the low-level calls to your DAQ's API.
            // You will be given the channel numbers and values. Here, we will just print
            // the output to the command line.
            for (std::size_t i = 0; i < n; ++i)
                print("Wrote {}[{}]: {} V", name(), chs[i], volts[i]);
            return true;
        };
        // Connect the lambda
        connect_write(*this, write_impl);

        // Implement other Buffers, Registers, etc:
        auto range_write_impl = [this](const ChanNum* chs, const Range<Volts>* ranges,
                                       std::size_t n) {
            for (std::size_t i = 0; i < n; ++i)
                print("Set {}}[{}] Range: ({} V, {} V)", name(), chs[i], ranges[i].min_val,
                      ranges[i].max_val);
            return true;
        };
        connect_write(ranges, range_write_impl);
    }

    // You have two options for making initalization calls on a Module. The first is on a
    // channel by channel basis, as they are added using #on_gain_channels:
    bool on_gain_channels(const ChanNums& chs) override {
        for (auto ch : chs)
            print("Initializing {}[{}]", name(), ch);
        return true;
    }
    // Likewise, you can finalize or free the Module on a channel by channel bases:
    bool on_free_channels(const ChanNums& chs) override {
        for (auto ch : chs)
            print("Finalizing {}[{}]", name(), ch);
        return true;
    }
    /// The other option for intialization/finalization are on a whole Module basis,
    /// either when the DAQ opens, closes, enables, or disables. Override these functions:
    bool on_daq_open() override {
        print("Initializing Module {}", name());
        return true;
    }
    bool on_daq_close() override {
        print("Finalizing Module {}", name());
        return true;
    }

    // Enable/disable, if needed
    // bool on_daq_enable() override;
    // bool on_daq_disable() override;

    // Often, I/O channels will have associative registers and other buffers. You can implement
    // these by adding attional Buffer types from Io.hpp . For example, here we expose a Register
    /// interface for setting the Analog Output ranges.
    Register<Range<Volts>> ranges;
};

/// Now, we will make a digital input Module
class MyDIModule : public DIModule {
public:
    MyDIModule(Daq& d, const ChanNums& allowed) : DIModule(d, allowed) {
        set_name(d.name() + ".DI");
    }
    /// TODO: similar procedures as above ...
};

/// Our final example Module is an incremental encoder
class MyEncoderModule : public EncoderModuleBasic {
public:
    MyEncoderModule(Daq& d, const ChanNums& allowed) : EncoderModuleBasic(d, allowed) {
        set_name(d.name() + ".encoder");
    }
    /// TODO: similar procesdures as above ...
};

/// Finally, we bring all our Modules together with a DAQ class.
class MyDaq : public Daq {
public:
    // Constructor
    MyDaq() :
        Daq("my_daq"),
        AO(*this, {0, 1, 2, 3, 4, 5}),  // 6 allowed channels
        DI(*this, {0, 1, 2, 3, 4, 5}),  // 6 allowed channels
        encoder(*this, {0, 1})          // 2 allowed channels
    {
        // open
        open();
        // Often, DIOs, encoders, PWMs, I2C, etc. share pins.
        // You can establish a pin sharing relationship. In this example,
        // DI shares pins 0,1,2 with encoder 0 (i.e. for A0,B0,Z0) and
        /// pins 3,4,5 with encoder 1 (i.e. for A1,B1,Z1). When the user
        /// sets the channel numbers of either DI or encoder, the
        /// shared pins will be freed from the other, and gained on the
        /// recipient. You implement this functionality in #on_gain_channels
        /// and #on_free_channels (see above).
        create_shared_pins(&DI, &encoder, {{{0, 1, 2}, {0}}, {{3, 4, 5}, {1}}});

        // set the inital channels for each Module
        AO.set_channels({0, 1, 2, 3, 4, 5});
        DI.set_channels({0, 1, 2, 3, 4, 5});
        encoder.set_channels({});  /// initially no channels, since they're all on DI
    }
    ~MyDaq() {
        if (is_enabled())
            disable();
        if (is_open())
            close();
    }
    bool on_daq_open() override {
        /// Open communications using your DAQ's API
        print("Opening DAQ {}", name());
        return true;
    }
    bool on_daq_close() override {
        /// Close communications using your DAQ's API
        print("Closing DAQ {}", name());
        return true;
    }
    bool on_daq_enable() override {
        print("Enabling DAQ {}", name());
        return true;
    }
    bool on_daq_disable() override {
        print("Disabling DAQ {}", name());
        return true;
    }

public:
    /// Expose your DAQ's Modules as public members
    MyAOModule      AO;
    MyDIModule      DI;
    MyEncoderModule encoder;
};

int main(int argc, char const* argv[]) {
    MahiLogger->set_max_severity(Verbose);
    print("---Construction---");
    MyDaq daq;
    print("---Info---");
    print_info(daq);
    daq.AO.enable_values.set({1, 2, 3, 4, 5, 6});
    daq.AO.disable_values.set({6, 5, 4, 3, 2, 1});
    print("---Enable---");
    daq.enable();
    print("---Siwtch Channels---");
    daq.encoder.set_channels({0, 1});
    daq.DI.set_channels({3, 4, 5});
    print("---Disable---");
    // daq.AO.write(1,5);
    return 0;
}
