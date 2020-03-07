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
#include <Mahi/Daq/Input.hpp>

namespace mahi {
namespace daq {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QuanserDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class QuanserDI : public DigitalInput {
public:
    QuanserDI(QuanserDaq& daq, const ChanNums& channel_numbers);

    bool update() override;

    bool update_channel(ChanNum channel_number) override;

    std::vector<char>& get_quanser_values();

private:
    QuanserDaq& daq_;  ///< Reference to parent QDaq
    Buffer<char> quanser_values_;

    QuanserDI( const QuanserDI& ) = delete; // non construction-copyable
    QuanserDI& operator=( const QuanserDI& ) = delete; // non copyable
};

} // namespace daq
} // namespace mahi
