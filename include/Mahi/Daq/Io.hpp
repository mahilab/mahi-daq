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

#pragma once
#include <Mahi/Daq/Module.hpp>
#include <Mahi/Daq/Buffer.hpp>

namespace mahi {
namespace daq {

// This file implements common IO Modules and Module Interfaces for typical use
// (e.g analog/digital input/output). Your DAQ interface is not limied to the
// types listed here. Use these as examples for creating your own interfaces
// with the provided template mechanisms.

//==============================================================================
// BUILT IN MODULE INTERFACES
//==============================================================================

/// A buffer that can be publicly set with operator[]
template <typename T>
using SettableBuffer = ISet<Buffer<T>>;

/// A buffer that can be publicly get with operator[]. 
/// M is the owning Module that is responsible for updating the buffer contents.
template <typename T, typename M>
using GettableBuffer = Friend<IGet<Buffer<T>>,M>;

/// A buffer that can be publicly set with operator[] and an immediate write interface
template <typename T>
using WriteBuffer = IWrite<ISet<Buffer<T>>>;

/// A buffer that can be publicly get with operator[] and an immediate read interface
template <typename T>
using ReadBuffer = IRead<IGet<Buffer<T>>>;

/// A buffer than can be publicaly set and get with operator[] and an immediate read/write
/// interface
template <typename T>
using ReadWriteBuffer = IRead<IWrite<ISet<Buffer<T>>>>;

/// A Buffer that exposes only immediate mode write functionality, e.g. for setting registers
template <typename T>
using Register = IWrite<IGet<Buffer<T>>>;

//==============================================================================
// INPUT MODULES
//==============================================================================

/// A DAQ Module that is also ReadBuffer. Used for generic inputs.
/// It will automatically  read on calls to Daq::read_all unless
/// the bool memeber read_with_all is manually set to false.
template <typename T>
class InputModule : public ChanneledModule, public ReadBuffer<T> {
public:
    /// Typedef this type for convenienve
    typedef InputModule<T> This;
    /// Constructor
    InputModule(Daq& daq, const ChanNums& allowed) :
        ChanneledModule(daq, allowed), ReadBuffer<T>(*this, T()) {
        this->read_with_all = true;
    }
    /// Destructor
    virtual ~InputModule() {}
};

/// Convenience type for analog input DAQ Module interfaces
typedef InputModule<Voltage> AIModule;

/// Convenience type for digital input DAQ Module interfaces
typedef InputModule<Logic> DIModule;

//==============================================================================
// OUTPUT MODULES
//==============================================================================

/// A DAQ Module that is also WriteBuffer. Used for generic outputs.
/// It will automatically  write on calls to Daq::write_all unless
/// the bool memeber write_with_all is manually set to false.
template <typename T>
class OutputModule : public ChanneledModule, public WriteBuffer<T> {
public:
    /// Typedef this type for convenienve
    typedef OutputModule<T> This;
    /// Constructor
    OutputModule(Daq& daq, const ChanNums& allowed) :
        ChanneledModule(daq, allowed),
        WriteBuffer<T>(*this, T()),
        enable_values(*this, T()),
        disable_values(*this, T()) {
        this->write_with_all = true;
    }
    /// Destructor
    virtual ~OutputModule() {}
    /// A buffer of values to be set when the Module's DAQ is enabled
    SettableBuffer<T> enable_values;
    /// A buffer of values to be set when the Module's DAQ is disabled
    SettableBuffer<T> disable_values;

protected:
    bool on_daq_enable() override { return this->write(this->enable_values.get()); }
    bool on_daq_disable() override { return this->write(this->disable_values.get()); }
};

/// Convenience type for analog output DAQ Module interfaces
typedef OutputModule<Voltage> AOModule;

/// Convenience type for digital output DAQ Module interfaces
typedef OutputModule<Logic> DOModule;

//==============================================================================
// PWM MODULES
//==============================================================================

/// A basic PWM output module which writes a duty cycle in the range of 0 to 1.
class PwmModuleBasic : public ChanneledModule, public WriteBuffer<double> {
public:
    PwmModuleBasic(Daq& daq, const ChanNums& allowed) :
        ChanneledModule(daq, allowed), WriteBuffer<double>(*this, 0) {
        write_with_all = true;
    }
};

//==============================================================================
// ENCODER MODULES
//==============================================================================

/// A incremental Encoder DAQ Module interface. It itself is a ReadWriteBuffer<Count>,
/// and should be configured to read/write encoder counts. Reading should get the
/// current value of the encoder channel, while writing should set the value. Most
/// DAQs should allow setting the the counts, for zeoring purposes.
///
/// It automatically reads on calls to Daq::read_all unless the bool member
/// read_with_all is manually set to false. It does NOT automatically write, since
/// that is not a realistic use case.
class EncoderModuleBasic : public ChanneledModule, public ReadWriteBuffer<Counts> {
public:
    EncoderModuleBasic(Daq& daq, const ChanNums& allowed) :
        ChanneledModule(daq, allowed), ReadWriteBuffer<Counts>(*this, 0) {
        read_with_all = true;
    }
    /// Zeros all encoder channels.
    bool zero() { return write(std::vector<Counts>(channels_internal().size())); }
    /// Zeros a single encoder channel.
    bool zero(ChanNum channel) { return write(channel, 0); }
};

/// A incremental Encoder DAQ Module interface. It itself is a ReadWriteBuffer<Count>,
/// and should be configured to read/write encoder counts. Reading should get the
/// current value of the encoder channel, while writing should set the value. Most
/// DAQs should allow setting the the counts, for zeoring purposes.
///
/// It automatically reads on calls to Daq::read_all unless the bool member
/// read_with_all is manually set to false. It does NOT automatically write, since
/// that is not a realistic use case.
///
/// The class has a built in WriteBuffer for setting the encoder quadrature
/// mode (see QuadMode for possible values). CRTP inherit from this to gain access
/// to the #quadratues Register, and implement its callbacks using your DAQ API if
/// you DAQ supports different modes. Catch unsupported quadrature modes and Log
/// an error when the are passed by the user.
///
/// For convenience, it also contains an interface for automatically converting
/// counts to positional units. Users define a units/per count value by setting
/// the #units buffer. They can then retrieve the converted position from the
/// #converted buffer after an encoder channel is read. The formula for the
/// conversion is: {counts * unit per count / quadratue factor}. The quadrature
/// factor is automatically obtained from the current value in the #quadratures
/// WriteBuffer. Example usage is as follows:
///
/// 1) encoder.units[0] = 360.0 / 1024; // user sets 360 degrees per 1024 counts
/// 2) auto cnt = encoder.read(0);      // user reads ch 0, gets 256
/// 3) auto pos = encoder.converted[0]  // will be 22.5 degrees (assuming 4x quadrature)
class EncoderModule : public EncoderModuleBasic {
public:
    /// Constructor.
    EncoderModule(Daq& daq, const ChanNums& allowed) :
        EncoderModuleBasic(daq, allowed),
        quadratures(*this, QuadMode::X4),
        units(*this, 1),
        converted(*this, 0) {
        // Updates positions after read and write
        auto convert = [this](const ChanNum* chs, const Counts* counts, std::size_t n) {
            for (std::size_t i = 0; i < n; ++i)
                converted.buffer(chs[i]) = static_cast<double>(counts[i]) * units[chs[i]] /
                                           static_cast<double>(quadratures[chs[i]]);
        };
        connect_post_read(*this, convert);
        connect_post_write(*this, convert);
    }
    /// The quadrature factor settings for each channel.
    Register<QuadMode> quadratures;
    /// The user defined units per count for each channel (e.g. 360 degrees / 1024 counts)
    SettableBuffer<double> units;
    /// The converted positions in the units defined by the user, i.e.
    /// {count * unit_per_count / quadratue factor} (see above)
    /// It is automatically update when the encoder is read or written
    GettableBuffer<double,EncoderModule> converted;
};

}  // namespace daq
}  // namespace mahi