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
#include <Mahi/Daq/Quanser2/QuanserHandle.hpp>
#include <Mahi/Daq/Quanser2/QuanserOther.hpp>

namespace mahi {
namespace daq {

class QuanserDaq;

/// Implements a Quanser incremental encoder module
class QuanserEncoder : public Fused<EncoderModule<QuanserEncoder>,QuanserDaq> {
public:
    /// Constructor
    QuanserEncoder(QuanserDaq& d, QuanserHandle& h, const ChanNums& chs);
private:
    QuanserHandle& m_h;
};

/// Increments a Quanser incremental encoder velocity estimation module
class QuanserEncoderVelocity : public QuanserOtherInput {
public:
    /// Constructor
    QuanserEncoderVelocity(QuanserDaq& d, QuanserHandle& h, QuanserEncoder& e, const ChanNums& chs);
    /// The computed velocities in the units defined by the user,
    /// i.e. [counts_per_sec * unit_per_count / quadratue factor]
    /// It is automatically update when the the velocity is read. 
    /// The units and quadrature factors are pulled from the accompanying QuanserEncoder.
    Fused<InputBuffer<double>,QuanserEncoderVelocity> converted;
private:
    /// Quanser encoder velocity channels awkwardly start at 14000 instead of 0, 
    /// therefore we apply a transformation so that they can be accessed starting at 0.
    virtual ChanNum transform_channel_number(ChanNum public_facing) const override;
    QuanserEncoder& m_e;
};

} // namespace daq
} // namespace mahi