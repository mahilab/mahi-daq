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
#include <mahi/daq/InputOutput.hpp>

namespace mahi {
namespace daq {

class MyRioConnector;
class MyRioEncoder;
class MyRioI2C;

/// myRIO Digital Input/Output Module
class MyRioDIO : public DigitalInputOutput, NonCopyable {
public:

    /// Updates a single channel
    bool update_channel(ChanNum channel_number) override;

    /// Sets the direction of a single channel
    bool set_direction(ChanNum channel_number, Direction direction) override;

private:

    friend class MyRioConnector;
    friend class MyRioEncoder;
    friend class MyRioI2C;

    void sync();

    MyRioDIO(MyRioConnector& connector, const ChanNums& channel_numbers);

private:

    const MyRioConnector& connector_; ///< connector this module is on

    // NI FPGA Registers
    ChanNum_t sysselect_;
    std::vector<ChanNum_t> dirs_;
    std::vector<ChanNum_t> ins_;
    std::vector<ChanNum_t> outs_;

};

} // namespace daq
} // namespace mahi
