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
#include <mahi/daq/Output.hpp>

namespace mahi {
namespace daq {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QuanserDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class QuanserDO : public DigitalOutput {
public:
    QuanserDO(QuanserDaq& daq, const ChanNums& channel_numbers);

    bool update() override;

    bool update_channel(ChanNum channel_number) override;

    std::vector<char>& get_quanser_values();

    bool set_expire_values(const std::vector<Logic>& expire_values) override;

    bool set_expire_value(ChanNum channel_number, Logic expire_value) override;

private:
    QuanserDaq& daq_;  ///< Reference to parent QDaq
    Buffer<char> quanser_values_;

    QuanserDO( const QuanserDO& ) = delete; // non construction-copyable
    QuanserDO& operator=( const QuanserDO& ) = delete; // non copyable
};

} // namespace daq
} // namespace mahi