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
#include <mahi/daq/Watchdog.hpp>

namespace mahi {
namespace daq {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class S826;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a hardware watchdog timer
class S826Watchdog : public Watchdog {
public:

    /// Destructor. Stops the watchdog if watching.
    ~S826Watchdog();

    bool start() override;

    bool kick() override;

    bool stop() override;

    bool is_expired() override;

    bool clear() override;

private:

    friend class S826;

    /// Private constructor used by S826
    S826Watchdog(S826& daq, util::Time timeout);

private:
    S826& s826_;  ///< Reference to parent QDaq
};

} // namespace daq
} // namespace mahi
