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
class DaqBase : public util::Device {
public:
    /// Constructor
    DaqBase();
    /// Destructor. By default, this closes the DAQ.
    virtual ~DaqBase();
    /// Reads all readable ModuleInterfaces known to this DAQ if they allow it
    virtual bool read_all();
    /// Reads all writeable ModuleInterfaces know to this DAQ if they allow it
    virtual bool write_all();
    /// Returns the number of modules on this DAQ
    const std::vector<Module*>& modules() const;
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
