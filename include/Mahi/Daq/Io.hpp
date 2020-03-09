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
class OutputBuffer : public WriteAccess<ModuleInterface<T>> {
public:
    using WriteAccess<ModuleInterface<T>>::WriteAccess;
};

/// A buffer that can be publicly read with operator[]
template <typename T>
class InputBuffer : public ReadAccess<ModuleInterface<T>> {
public:
    using ReadAccess<ModuleInterface<T>>::ReadAccess;
};

/// A buffer that can be publicly written with operator[] and an immediate write interface
template <typename T>
class WriteBuffer : public WriteImmediate<WriteAccess<ModuleInterface<T>>> {
public:
    using WriteImmediate<WriteAccess<ModuleInterface<T>>>::WriteImmediate;
};

/// A buffer that can be publicly read with operator[] and an immediate read interface
template <typename T>
class ReadBuffer : public ReadImmediate<ReadAccess<ModuleInterface<T>>> {
public:
    using ReadImmediate<ReadAccess<ModuleInterface<T>>>::ReadImmediate;
};

/// A buffer than can be publicaly read and written with operator[] and an immediate read/write interface
template <typename T>
class ReadWriteBuffer : public ReadImmediate<WriteImmediate<WriteAccess<ModuleInterface<T>>>> {
public:
    using ReadImmediate<WriteImmediate<WriteAccess<ModuleInterface<T>>>>::ReadImmediate;
};

/// A ModuleInterface that exposes only immediate mode write functionality, e.g. for setting registers
template <typename T>
class Register : public WriteImmediate<ReadAccess<ModuleInterface<T>>> {
public:
    using WriteImmediate<ReadAccess<ModuleInterface<T>>>::WriteImmediate;
};

//=============================================================================
// BUILT IN MODULES
//=============================================================================

/// A DAQ input Module that behaves publcialy like ReadBuffer
template <typename T>
class InputModule : public Module, public ReadBuffer<T> {
public:
    /// Typedef this type for convenienve
    typedef InputModule<T> This;
    /// Constructor
    InputModule(DaqBase& daq) : 
        Module(daq), ReadBuffer<T>(*this, T()) 
    { read_with_all = true; }
    /// Destructor
    virtual ~InputModule() { }
};

/// An analog input DAQ Module interface
typedef InputModule<Voltage> AIModule;

/// A digital input DAQ Module interface
typedef InputModule<Logic> DIModule;

/// A DAQ output Module that behaves publcialy like WriteBuffer
template <typename T>
class OutputModule : public Module, public WriteBuffer<T> {
public:
    /// Typedef this type for convenienve
    typedef OutputModule<T> This;
    /// Constructor
    OutputModule(DaqBase& daq) : 
        Module(daq), WriteBuffer<T>(*this, T()), enable_values(*this, T()), disable_values(*this, T()) 
    { write_with_all = true; }
    /// Destructor
    virtual ~OutputModule() { }
    /// A buffer of values to be set when the Module's DAQ is enabled
    Fused<OutputBuffer<Voltage>,This> enable_values;
    /// A buffer of values to be set when the Module's DAQ is disabled
    Fused<OutputBuffer<Voltage>,This> disable_values; 
protected:
    bool on_enable() override { return this->write(this->enable_values.buffer()); }   
    bool on_disable() override { return this->write(this->disable_values.buffer()); }
};

/// A digital output DAQ Module interface
typedef OutputModule<Voltage> AOModule;

/// A digital output DAQ Module interface
typedef OutputModule<Logic> DOModule;

/// A incremental Encoder DAQ Module interace. CRTP inherit from this to gain access to quadratues
template <class Crtp>
class EncoderModule : public Module, public ReadWriteBuffer<int> {
public:
    friend Crtp;
    /// Typedef this type for convenienve
    typedef EncoderModule<Crtp> This;
    /// Constructor
    EncoderModule(DaqBase& daq) : 
        Module(daq),
        ReadWriteBuffer<int>(*this, 0),
        quadratures(*this, QuadFactor::X4), units(*this, 1), converted(*this, 0)
    { 
        // Updates positions after read and write
        auto convert = [this](const ChanNum * chs, const int* counts, std::size_t n) {
            for (int i = 0; i < n; ++i) 
                converted.buffer(chs[i]) = static_cast<double>(counts[i]) * units[chs[i]] / static_cast<double>(quadratures[chs[i]]);
        };
        this->post_read.connect(convert);
        this->post_write.connect(convert);  
        read_with_all = true;      
    }
    /// Destructor
    virtual ~EncoderModule() { }
    /// Zeros all encoder channels
    bool zero() { return write(std::vector<int>(channels_internal().size())); }
    /// Zeros a single encoder channel
    bool zero(ChanNum channel) { return write(channel, 0); }
    /// The quadrature factor settings for each channel
    Fused<Register<QuadFactor>,Crtp> quadratures;
    /// The user defined units per count for each channel (e.g. 360 degrees / 1024 counts)
    Fused<OutputBuffer<double>,This> units;
    /// The converted positions in the units defined by the user, i.e. [count * unit_per_count / quadratue factor] (see above)
    /// It is automatically update when the encoder is read or written
    Fused<InputBuffer<double>,This> converted;
};

} // namespace daq
} // namespace mahi

// #include <Mahi/Daq/Detail/Input.inl>
