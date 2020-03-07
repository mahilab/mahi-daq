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

#include <Mahi/Daq/Output.hpp>

namespace mahi {
namespace daq {

class S826;

class S826AO : public AnalogOutput {
public:

    bool update_channel(ChanNum channel_number) override;

private:
    friend class S826;

    /// Private constructor used by S826
    S826AO(S826& s826);

    /// Caled by S826 when it opens
    bool on_open();

private:
    S826& s826_;

    S826AO( const S826AO& ) = delete; // non construction-copyable
    S826AO& operator=( const S826AO& ) = delete; // non copyable
};

} // namespace daq
} // namespace mahi