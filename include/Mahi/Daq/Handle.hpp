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

namespace mahi {
namespace daq {

// Handles are convenience wrappers around a single Module channel. They are
// constructed from the Module of choice and a channel number. They are
// meant to be passed around by value, and allow you to provide your classes
// with a sliced view of Module without passing multiple references to its
// various Buffers. All Handles provide read/write (i.e. physical) and
// get/set functionality (i.e. software). In most scenarios, you probably don't
// want to read/write several individual handles, as this is not efficient.
// Instead, you you should read/write the entire DAQ or DAQ module in your
// main sampling loop, then use the get/set functions of the Handles to get or
// the buffer value, e.g.:
//
// while(...) {
//     daq.read_all(); // or daq.AI.read()
//     ...
//     volts = ai_handle.get_volts();
//     ao_handle.set_volts(volts);
//     ...
//     daq.write_all(); // or daq.AO.write()
// }
//
// Feel free to extended these classes to add additional functionality!

/// A single-channel view into an AIModule
class AIHandle {
public:
    /// Default Constructor (invalid handle)
    AIHandle() {}
    /// Constructor
    AIHandle(AIModule& mod, ChanNum ch) : m_mod(&mod), m_ch(ch) {}
    /// Physically reads the voltage into the software buffer.
    inline bool read() { return m_mod->read(m_ch); }
    /// Physically reads the voltage into the software buffer and returns the value.
    inline Volts read_volts() { return read() ? get_volts() : 0; }
    /// Returns the current value in the software buffer.
    inline Volts get_volts() { return m_mod->get(m_ch); }

protected:
    AIModule* m_mod;
    ChanNum   m_ch;
};

/// A single-channel view into a DIModule
class DIHandle {
public:
    /// Default Constructor (invalid handle)
    DIHandle() {}
    /// Constructor
    DIHandle(DIModule& mod, ChanNum ch) : m_mod(&mod), m_ch(ch) {}
    /// Physically reads the TTL level into the software buffer.
    inline bool read() { return m_mod->read(m_ch); }
    /// Physically reads the TTL level into the software buffer and returns the value.
    inline TTL read_level() { return read() ? get_level() : TTL_ERROR; }
    /// Returns the current value in the software buffer.
    inline TTL get_level() { return m_mod->get(m_ch); }
    /// Returns true if the current software buffer value is TTL_LOW.
    inline bool is_low() { return get_level() == TTL_LOW; }
    /// Returns true if the current sofware buffer value is TTL_HIGH.
    inline bool is_high() { return get_level() == TTL_HIGH; }

protected:
    DIModule* m_mod;
    ChanNum   m_ch;
};

/// A single-channel view into an AOModule
class AOHandle {
public:
    /// Default Constructor (invalid handle)
    AOHandle() {}
    /// Constructor
    AOHandle(AOModule& mod, ChanNum ch) : m_mod(&mod), m_ch(ch) {}
    /// Physically writes the voltage currently in the software buffer
    inline bool write() { return m_mod->write(m_ch, m_mod->get(m_ch)); }
    /// Physically writes the passed value
    inline bool write_volts(Volts v) { return m_mod->write(m_ch, v); }
    /// Sets the current software buffer value
    inline bool set_volts(Volts v) { return m_mod->set(m_ch, v); }
    /// Returns the current value in the software buffer.
    inline Volts get_volts() { return m_mod->get(m_ch); }
    /// Sets the enable value
    inline bool set_enable(Volts v) { return m_mod->enable_values.set(m_ch, v); }
    /// Sets the disable value
    inline bool set_disable(Volts v) { return m_mod->disable_values.set(m_ch, v); }

protected:
    AOModule* m_mod;
    ChanNum   m_ch;
};

/// A single-channel view into a DOModule
class DOHandle {
public:
    /// Default Constructor (invalid handle)
    DOHandle() {}
    /// Constructor
    DOHandle(DOModule& mod, ChanNum ch) : m_mod(&mod), m_ch(ch) {}
    /// Physically writes the level currently in the software buffer
    inline bool write() { return m_mod->write(m_ch, m_mod->get(m_ch)); }
    /// Physically writes the passed value
    inline bool write_level(TTL l) { return m_mod->write(m_ch, l); }
    /// Physically writes the output to TTL_HIGH
    inline bool write_high() { return m_mod->write(m_ch, TTL_HIGH); };
    /// Physically writes the output to TTL_LOW
    inline bool write_low() { return m_mod->write(m_ch, TTL_LOW); }
    /// Sets the current software buffer value
    inline bool set_level(TTL l) { return m_mod->set(m_ch, l); }
    /// Sets the current software buffer value to TTL_HIGH
    inline bool set_high() { return m_mod->set(m_ch, TTL_HIGH); };
    /// Sets the current software buffer value to TTL_LOW
    inline bool set_low() { return m_mod->set(m_ch, TTL_LOW); }
    /// Flips the current software value (i.e. TTL_LOW becomes TTL_HIGH, vice versa)
    inline bool flip() { return m_mod->set(m_ch, TTL_HIGH - m_mod->get(m_ch)); }
    /// Flips the current software value (i.e. TTL_LOW becomes TTL_HIGH, vice versa)
    inline bool flip_write() { return m_mod->write(m_ch, TTL_HIGH - m_mod->get(m_ch)); }
    /// Sets the enable value
    inline bool set_enable(TTL l) { return m_mod->enable_values.set(m_ch, l); }
    /// Sets the disable value
    inline bool set_disable(TTL l) { return m_mod->disable_values.set(m_ch, l); }

protected:
    DOModule* m_mod;
    ChanNum   m_ch;
};

/// A single-channel view into an EncoderModule
class EncoderHandle {
public:
    /// Default Constructor (invalid handle)
    EncoderHandle() {}
    /// Constructor
    EncoderHandle(EncoderModule& mod, ChanNum ch) : m_mod(&mod), m_ch(ch) {}
    /// Physically reads the counts into the software buffer.
    inline bool read() { return m_mod->read(m_ch); }
    /// Physically reads the counts into the software buffer and returns the value.
    inline Counts read_counts() { return read() ? get_counts() : 0; }
    /// Returns the current counts in the software buffer.
    inline Counts get_counts() const { return m_mod->get(m_ch); }
    /// Physically reads the counts and returns the converted position.
    inline double read_pos() { return read() ? get_pos() : 0; }
    /// Returns the converted position based on the current counts, quad mode, and units.
    inline double get_pos() const { return m_mod->positions.get(m_ch); }
    /// Sets the units to be used in position conversions.
    inline bool set_units(double u) { return m_mod->units.set(m_ch, u); }
    /// Writes the quadrature mode that the DAQ will use.
    inline bool write_mode(QuadMode q) { return m_mod->modes.write(m_ch, q); }
    /// Physically writes the encoder counts on the DAQ.
    inline bool write_counts(Counts cnts) { return m_mod->write(m_ch, cnts); }
    /// Physically zeros the encoder counts on the DAQ.
    inline bool zero() { return m_mod->zero(m_ch); }

protected:
    EncoderModule* m_mod;
    ChanNum        m_ch;
};

}  // namespace daq
}  // namespace mahi