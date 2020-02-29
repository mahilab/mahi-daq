// MIT License
//
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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
#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Core/Types.hpp>
#include <MEL/Core/Device.hpp>
#include <bitset>

namespace mahi {
namespace daq {

class MyRioMxp;

class MyRioI2C : public NonCopyable, public Device {
public:

    void set_slave(const std::string& address);
    void set_slave(std::bitset<7> address);

    void send(const void* data, std::size_t size);

private:

    friend class MyRioMxp;

    MyRioI2C(MyRioMxp& connector);

    virtual bool on_enable() override;
    virtual bool on_disable() override;
    void sync();

    void execute();

private:

    MyRioMxp& connector_;

    // NI FPGA Registers
    ChanNum_t sysselect_; ///< system select register
    ChanNum_t cnfg_;      ///< configuration register
    ChanNum_t addr_;      ///< slave address register
    ChanNum_t cntr_;      ///< counter register
    ChanNum_t dato_;      ///< data-out reigster
    ChanNum_t dati_;      ///< data-in register
    ChanNum_t stat_;      ///< status register
    ChanNum_t ctrl_;      ///< control reigster
    ChanNum_t go_;        ///< execute register

};

} // namespace daq
} // namespace mahi