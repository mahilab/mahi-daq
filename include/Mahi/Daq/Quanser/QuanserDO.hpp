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
#include <Mahi/Daq/Quanser/QuanserHandle.hpp>

namespace mahi {
namespace daq {

class QuanserDaq;

class QuanserDO : public DOModule {
public:
    QuanserDO(QuanserDaq& d, QuanserHandle& h, bool bidirectional, const ChanNums& allowed);
    Register<Logic> expire_values;
private:
    friend QuanserDaq;
    bool on_gain_channels(const ChanNums& chs) override;
    QuanserHandle& m_h;
    bool m_bidirectional;
};

} // namespace daq
} // namespace mahi