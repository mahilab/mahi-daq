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

#include <Mahi/Daq/ModuleArray.hpp>
#include <algorithm>
#include <functional>

namespace mahi {
namespace daq {

/// A container of values associated with a Module, indexible by channel number
template <typename T>
class Buffer : public ModuleArray<T> {
public:

    /// Constructor
    Buffer(ModuleBase* module, T default_value = T());

    /// Write access indexed by channel number
    T& operator[](ChanNum channel_number);

    /// Gets non-const reference to raw values array
    std::vector<T>& get();

    /// Gets a const reference to raw values array
    const std::vector<T>& get() const;

    /// Sets raw values array
    void set(const std::vector<T>& values);
};

template <typename T, class TModule >
class BufferR : public ModuleArray<T> {
public:

    /// Constructor
    BufferR(ModuleBase* module, T default_value = T()) :
        ModuleArray<T>(module, default_value)
    { 
        on_read_all = nullptr;
        on_read_one = nullptr;
    }

    /// Gets non-const reference to raw values array
    bool read() {
        if (on_read_all)
            return on_read_all(m_module->channel_numbers(), m_values);
        return true;
    }

    bool read(ChanNum channel) {
        if (!m_module->validate_channel_number(channel))
            return false;
        if (on_read_one)
            return on_read_one(channel, m_values[index(channel)]);
        return true;
    }

private:
    friend TModule;
    std::function<bool(const ChanNums&, std::vector<T>&)> on_read_all;
    std::function<bool(const ChanNum, T&)>                on_read_one;
};

template <typename T, class TModule>
class BufferW : public ModuleArray<T> {
public:

    /// Constructor
    BufferW(ModuleBase* module, T default_value = T()) :
        ModuleArray<T>(module, default_value)
    { 
        on_write_all = nullptr;
        on_write_one = nullptr;
    }

    /// Write access indexed by channel number
    T& operator[](ChanNum channel_number) {
        return m_values[index(channel_number)];
    }

    bool write() {
        if (on_write_all)
            return on_write_all(m_module->channel_numbers(), m_values);
        return true;
    }

    /// Gets non-const reference to raw values array
    bool write(const std::vector<T>& values) {
        if (!m_module->validate_channel_count(values.size()))
            return false;
        if (on_write_all) {
            if (on_write_all(m_module->channel_numbers(), values)) {
                m_values = values;
                return true;
            }
            return false;
        }
        else {
            m_values = values;
            return true;
        }
    }

    bool write(ChanNum channel, T value) {
        if (!m_module->validate_channel_number(channel))
            return false;
        if (on_write_one) {
            if (on_write_one(channel, value))
                m_values[index(channel)] = value;
        }
        else {
            m_values[index(channel)] = value;
            return true;
        }
    }
    
private:
    friend TModule;
    std::function<bool(const ChanNums&, const std::vector<T>&)> on_write_all;
    std::function<bool(const ChanNum, T)>                       on_write_one;
};




} // namespace daq
} // namespace mahi

#include <Mahi/Daq/Detail/Buffer.inl>
