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
#include <Mahi/Daq/Encoder.hpp>

namespace mahi {
namespace daq {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QuanserDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Quanser implementation of Encoder
class QuanserEncoder : public Encoder {
public:
    class Channel;

    QuanserEncoder(QuanserDaq& daq, const ChanNums& channel_numbers, bool has_velocity);

    bool update() override;

    bool update_channel(ChanNum channel_number) override;

    bool reset_counts(const std::vector<int>& counts) override;

    bool reset_count(ChanNum channel_number, int count) override;

    bool set_quadrature_factors(const std::vector<QuadFactor>& factors) override;

    bool set_quadrature_factor(ChanNum channel_number, QuadFactor factor) override;

    /// Set whether the Encoder enables velocity estimation
    bool has_velocity() const override;

    /// Returns the Quanser encoder velocity channels
    const ChanNums velocity_channel_numbers();

private:

    QuanserDaq& daq_;                          ///< Reference to parent QDaq
    Buffer<ChanNum> velocity_channel_numbes_;  ///< Converted channel numbers
    bool has_velocity_;                        ///< True if Encoder module has velocity estimation

};

} // namespace daq
} // namespace mahi
