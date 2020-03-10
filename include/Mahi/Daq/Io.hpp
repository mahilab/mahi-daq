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
#include <Mahi/Daq/ModuleInterface.hpp>

namespace mahi {
namespace daq {

// This file implements common IO Modules and Module Interfaces for typical use
// (e.g analog/digital input/output). Your DAQ interface is not limied to the 
// types listed here. Use these as examples for creating your own interfaces 
// with the provided template mechanisms.

//=============================================================================
// FUSION
//=============================================================================

/// Exposes the protected members of Protected to Beneficiary 
template <class Protected, class Beneficiary>
class Fused : public Protected {
    using Protected::Protected;
    friend Beneficiary;
};

// The types below can be inherited from or used as public member variables 
// to create a desired DAQ interface. To keep the public interface clean and
// prevent misuse, these types by default protect their internal buffers and
// callback Events. Your DAQ interface will obviously need to accesss these
// to implement functionality. This is what the Fused class is for. For
// instances, if you need a public output buffer interface in you class, 
// fuse it with the class itself:
//
// class MyModule : public Module {
//   public:
//     Fused<OutputBuffer<int>,MyModule> buffer;
// };
//
// The internal buffers and callbacks of buffer will remain hidden to clients,
// but MyModule will gain access to them so that it can implement it.
//
// If more than one class will need access, chain the fusions:
//
// Fused<Fused<WriteBuffer<double>,MyModule>,MyDaq>
//
// If you need to extend one of the classes below, it is not necessary to fuse
// because your class will automatically inherit the procted members. However,
// if another class will need access to your extended version, you may chose to
// fuse it:
//
// class MyAoModule : public Fused<AOModule,MyDaq> {
//   ...
// }
//
// class MyDaq {
//   public
//     MyAoModule ao; 
// }
//
// Here, MyAoModule will be able to access AOModule's buffers through inheritance, 
// and MyDaq will be able to access them through fusion. Note, it would have also
// been possible to perform the fusion inside of MyDaq as well, as in example 1.

//=============================================================================
// BUILT IN MODULE INTERFACES
//=============================================================================

/// A buffer that can be publicly written with operator[]
template <typename T>
class OutputBuffer : public SetAccess<ModuleInterface<T>> {
public:
    using SetAccess<ModuleInterface<T>>::SetAccess;
};

/// A buffer that can be publicly read with operator[]
template <typename T>
class InputBuffer : public GetAccess<ModuleInterface<T>> {
public:
    using GetAccess<ModuleInterface<T>>::GetAccess;
};

/// A buffer that can be publicly written with operator[] and an immediate write interface
template <typename T>
class WriteBuffer : public WriteImmediate<SetAccess<ModuleInterface<T>>> {
public:
    using WriteImmediate<SetAccess<ModuleInterface<T>>>::WriteImmediate;
};

/// A buffer that can be publicly read with operator[] and an immediate read interface
template <typename T>
class ReadBuffer : public ReadImmediate<GetAccess<ModuleInterface<T>>> {
public:
    using ReadImmediate<GetAccess<ModuleInterface<T>>>::ReadImmediate;
};

/// A buffer than can be publicaly read and written with operator[] and an immediate read/write interface
template <typename T>
class ReadWriteBuffer : public ReadImmediate<WriteImmediate<SetAccess<ModuleInterface<T>>>> {
public:
    using ReadImmediate<WriteImmediate<SetAccess<ModuleInterface<T>>>>::ReadImmediate;
};

/// A ModuleInterface that exposes only immediate mode write functionality, e.g. for setting registers
template <typename T>
class Register : public WriteImmediate<ModuleInterface<T>> {
public:
    using WriteImmediate<ModuleInterface<T>>::WriteImmediate;
};

//=============================================================================
// INPUT MODULES
//=============================================================================

/// A DAQ Module that is also ReadBuffer. Used for generic inputs.
/// It will automatically  read on calls to Daq::read_all unless 
/// the bool memeber read_with_all is manually set to false.
template <typename T>
class InputModule : public ChannelsModule, public ReadBuffer<T> {
public:
    /// Typedef this type for convenienve
    typedef InputModule<T> This;
    /// Constructor
    InputModule(Daq& daq) : 
        ChannelsModule(daq), ReadBuffer<T>(*this, T()) 
    { read_with_all = true; }
    /// Destructor
    virtual ~InputModule() { }
};

/// Convenience type for analog input DAQ Module interfaces
typedef InputModule<Voltage> AIModule;

/// Convenience type for digital input DAQ Module interfaces
typedef InputModule<Logic> DIModule;

//=============================================================================
// OUTPUT MODULES
//=============================================================================

/// A DAQ Module that is also WriteBuffer. Used for generic outputs.
/// It will automatically  write on calls to Daq::write_all unless 
/// the bool memeber write_with_all is manually set to false.
template <typename T>
class OutputModule : public ChannelsModule, public WriteBuffer<T> {
public:
    /// Typedef this type for convenienve
    typedef OutputModule<T> This;
    /// Constructor
    OutputModule(Daq& daq) : 
        ChannelsModule(daq), WriteBuffer<T>(*this, T()), enable_values(*this, T()), disable_values(*this, T()) 
    { write_with_all = true; }
    /// Destructor
    virtual ~OutputModule() { }
    /// A buffer of values to be set when the Module's DAQ is enabled
    Fused<OutputBuffer<T>,This> enable_values;
    /// A buffer of values to be set when the Module's DAQ is disabled
    Fused<OutputBuffer<T>,This> disable_values; 
protected:
    bool on_daq_enable() override { return this->write(this->enable_values.buffer()); }   
    bool on_daq_disable() override { return this->write(this->disable_values.buffer()); }
};

/// Convenience type for analog output DAQ Module interfaces
typedef OutputModule<Voltage> AOModule;

/// Convenience type for digital output DAQ Module interfaces
typedef OutputModule<Logic> DOModule;

//=============================================================================
// ENCODER MODULE
//=============================================================================

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
template <class Crtp = void>
class EncoderModule : public ChannelsModule, public ReadWriteBuffer<Counts> {
public:
    friend Crtp;
    /// Typedef this type for convenienve.
    typedef EncoderModule<Crtp> This;
    /// Constructor.
    EncoderModule(Daq& daq) : 
        ChannelsModule(daq),
        ReadWriteBuffer<Counts>(*this, 0),
        quadratures(*this, QuadMode::X4), units(*this, 1), converted(*this, 0)
    { 
        // Updates positions after read and write
        auto convert = [this](const ChanNum * chs, const Counts* counts, std::size_t n) {
            for (int i = 0; i < n; ++i) 
                converted.buffer(chs[i]) = static_cast<double>(counts[i]) * units[chs[i]] / static_cast<double>(quadratures[chs[i]]);
        };
        this->post_read.connect(convert);
        this->post_write.connect(convert);  
        read_with_all = true;      
    }
    /// Destructor.
    virtual ~EncoderModule() { }
    /// Zeros all encoder channels.
    bool zero() { return write(std::vector<Counts>(channels_internal().size())); }
    /// Zeros a single encoder channel.
    bool zero(ChanNum channel) { return write(channel, 0); }
    /// The quadrature factor settings for each channel.
    Fused<WriteBuffer<QuadMode>,Crtp> quadratures;
    /// The user defined units per count for each channel (e.g. 360 degrees / 1024 counts)
    Fused<OutputBuffer<double>,This> units;
    /// The converted positions in the units defined by the user, i.e. 
    /// {count * unit_per_count / quadratue factor} (see above)
    /// It is automatically update when the encoder is read or written
    Fused<InputBuffer<double>,This> converted;
};

} // namespace daq
} // namespace mahi

// #include <Mahi/Daq/Detail/Input.inl>
