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
#include <Mahi/Util/Device.hpp>

namespace mahi {
namespace daq {

// Forward Declarations
class Readable;
class Writeable;
class Module;

/// A DAQ interface, the topmost level of the mahi::daq architecture
class Daq : public util::Device {
public:
    /// Constructor
    Daq(const std::string& name = "UNAMED_DAQ");
    /// Destructor.
    virtual ~Daq();
    /// Reads all readable ModuleInterfaces known to this DAQ if they allow it
    virtual bool read_all();
    /// Reads all writeable ModuleInterfaces know to this DAQ if they allow it
    virtual bool write_all();
    /// Returns the number of modules on this DAQ
    const std::vector<Module*>& modules() const;
protected:
    /// Called when the DAQ opens
    virtual bool on_daq_open() { return true; }
    /// Called when the DAQ closes
    virtual bool on_daq_close() { return true; }
    /// Called when the DAQ enables
    virtual bool on_daq_enable() { return true; }
    /// Called when the DAQ disables
    virtual bool on_daq_disable() { return true; }
private:
    /// Calls Daq::on_daq_open, then iteratively calls Module::on_daq_open 
    bool on_open() final;
    /// Iteratively calls Module::on_daq_close , then Daq::on_daq_close
    bool on_close() final;
    /// Calls Daq::on_daq_enable, then iteratively calls Module::on_daq_enable 
    bool on_enable() final;
    /// Iteratively calls Module::on_daq_enable , then Daq::on_daq_enable
    bool on_disable() final;
private:
    /// The Modules owned by this DAQ
    std::vector<Module*> m_modules;
    friend Module;
    /// The readable ModuleInerfaces indirectly owned by this DAQ
    std::vector<Readable*> m_readables;
    friend Readable;
    /// The writeable ModuleInterfaces indirectly owned by this DAQ
    std::vector<Writeable*> m_writeables;
    friend Writeable;
};

} // namespace daq
} // namespace mahi
