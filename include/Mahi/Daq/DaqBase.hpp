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
#include <unordered_map>

namespace mahi {
namespace daq {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class DaqBase : public util::Device {
public:

    /// Constructor
    DaqBase(const std::string& name);

    /// Destructor. By default, this closes the DAQ.
    virtual ~DaqBase();

    /// Updates all Input modules contained on the DAQ
    /// return TRUE if successful, FALSE otherwise
    virtual bool update_input();

    /// Updates all Output modules contained on the DAQ
    /// return TRUE if successful, FALSE otherwise
    virtual bool update_output();

};

} // namespace daq
} // namespace mahi
