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
#include <mahi/daq/DaqBase.hpp>
#include <mahi/daq/Input.hpp>
#include <mahi/daq/Output.hpp>
#include <mahi/daq/InputOutput.hpp>
#include <mahi/daq/Encoder.hpp>
#include <functional>

namespace mahi {
namespace daq {

class VirtualDaq;

//==============================================================================
// VIRTUAL AI
//==============================================================================

class VirtualAI : public AnalogInput {
public:
    VirtualAI(VirtualDaq& daq, const ChanNums& channel_numbers);
    bool update_channel(ChanNum channel_number) override;
public:
    Buffer<std::function<Voltage(void)>> sources;
private:
    VirtualDaq& daq_;
};

//==============================================================================
// VIRTUAL AO
//==============================================================================

class VirtualAO : public AnalogOutput {
public:
    VirtualAO(VirtualDaq& daq, const ChanNums& channel_numbers);
    bool update_channel(ChanNum channel_number) override;
public:
    Buffer<std::function<void(Voltage)>> sinks;
private:
    VirtualDaq& daq_;
};

//==============================================================================
// VIRTUAL DI
//==============================================================================

class VirtualDI : public DigitalInput {
public:
    VirtualDI(VirtualDaq& daq, const ChanNums& channel_numbers);
    bool update_channel(ChanNum channel_number) override;
public:
    Buffer<std::function<Logic(void)>> sources;
private:
    VirtualDaq& daq_;
};

//==============================================================================
// VIRTUAL DO
//==============================================================================

class VirtualDO : public DigitalOutput {
public:
    VirtualDO(VirtualDaq& daq, const ChanNums& channel_numbers);
    bool update_channel(ChanNum channel_number) override;
public:
    Buffer<std::function<void(Logic)>> sinks;
private:
    VirtualDaq& daq_;
};

//==============================================================================
// VIRTUAL ENCODER
//==============================================================================

class VirtualEncoder : public Encoder {
public:
    VirtualEncoder(VirtualDaq& daq, const ChanNums& channel_numbers);
    bool update_channel(ChanNum channel_number) override;
    bool reset_count(ChanNum channel_number, int count);
public:
    Buffer<std::function<int(void)>> sources;
private:
    VirtualDaq& daq_;
};

//==============================================================================
// VIRTUAL DAQ
//==============================================================================

/// Virtual DAQ for testing/prototyping the absence of actual hardware
class VirtualDaq : public DaqBase {
public:
    VirtualDaq(const std::string& name);
    ~VirtualDaq();

    bool update_input() override;
    bool update_output() override;
public:
    VirtualAI AI;
    VirtualAO AO;
    VirtualDI DI;
    VirtualDO DO;
    VirtualEncoder encoder;

protected:
    bool on_enable() override;
    bool on_disable() override;
    bool on_open() override;
    bool on_close() override;

protected:
    friend class VirtualAI;
    friend class VirtualAO;
    friend class VirtualDI;
    friend class VirtualDO;
    friend class VirtualEncoder;
};

} // namespace daq
} // namespace mahi
