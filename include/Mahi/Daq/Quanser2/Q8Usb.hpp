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
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <Mahi/Daq/Quanser2/QuanserAI.hpp>
#include <Mahi/Daq/Quanser2/QuanserAO.hpp>
#include <Mahi/Daq/Quanser2/QuanserDI.hpp>
#include <Mahi/Daq/Quanser2/QuanserDO.hpp>
#include <Mahi/Daq/Quanser2/QuanserEncoder.hpp>

namespace mahi {
namespace daq {

class Q8Usb : public QuanserDaq {
public:
    Q8Usb();
    Fused<QuanserAI,Q8Usb> AI;
    QuanserAO AO;
    QuanserDI DI;
    QuanserEncoder encoder;
    Fused<QuanserEncoderVelocity,Q8Usb> velocity;
};

} // namespace daq
} // namespace mahi