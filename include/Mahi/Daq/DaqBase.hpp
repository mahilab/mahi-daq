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
#include <typeindex>
#include <set>

namespace mahi {
namespace daq {

class Readable;
class Writeable;
class Module;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class DaqBase : public util::Device {
public:
    /// Constructor
    DaqBase();
    /// Destructor. By default, this closes the DAQ.
    virtual ~DaqBase();
    /// Reads all readable ModuleInterfaces owned
    virtual bool read();
    /// Reads all writeable ModuleInterfaces owned
    virtual bool write();
    /// Returns the number of modules on this DAQ
    std::size_t module_count() const;
private:
    friend Module;
    friend Readable;
    friend Writeable;
    /// The Modules owned by this DAQ
    std::vector<Module*> m_modules;
    /// The readable ModuleInerfaces indirectly owned by this DAQ
    std::vector<Readable*> m_readables;
    /// The writeable ModuleInterfaces indirectly owned by this DAQ
    std::vector<Writeable*> m_writeables;
};

} // namespace daq
} // namespace mahi
