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

#include <Mahi/Daq/Io.hpp>
#include <Mahi/Util/Timing/Time.hpp>

namespace mahi {
namespace daq {

class S826;

class S826Encoder : public EncoderModule<S826Encoder> {
public:
    Fused<InputBuffer<util::Time>,S826Encoder> timestamps;
private:
    friend class S826;
    S826Encoder(S826& s826, unsigned int board);
private:
    const unsigned int m_board;
};


} // namespace daq
} // namespace mahi