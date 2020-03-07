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
#include <Mahi/Daq/Types.hpp>
#include <Mahi/Util/NonCopyable.hpp>

namespace mahi {
namespace daq {

class ModuleBase;

/// Base class for Module array types
class ModuleArrayBase : util::NonCopyable {
public:
    /// Constructor
    ModuleArrayBase(ModuleBase* module);

    // Gets size of array (i.e. the number of Module channels)
    std::size_t size() const;

protected:

    friend class ModuleBase;

    /// Called by ModuleBase when channel numbers change
    virtual void change_channel_numbers(const ChanMap& old_map, const ChanMap& new_map) = 0;

    /// Returns vector index associated with channel number
    std::size_t index(ChanNum channel_number) const;

protected:

    ModuleBase* m_module; ///< pointer to parent module
};


template <typename T>
class ModuleArray : public ModuleArrayBase {
public:

    ModuleArray(ModuleBase* module, T default_value = T());

    /// Read access indexed by channel number
    const T& operator[](ChanNum channel_number) const;

    /// Sets the default value subsequent new values should be instantied with
    void set_default(T default_value);

    /// Overload stream operator
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const ModuleArray<U>& array);

protected:

    T m_default_value;
    std::vector<T> m_values;  ///< raw values

private:

    /// Called by ModuleBase when channel numbers change
    void change_channel_numbers(const ChanMap& old_map, const ChanMap& new_map) override;

};


} // namespace daq
} // namespace mahi

#include <Mahi/Daq/Detail/ModuleArray.inl>
