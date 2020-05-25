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
#include <Mahi/Daq/Daq.hpp>
#include <Mahi/Daq/Quanser/QuanserHandle.hpp>
#include <Mahi/Daq/Quanser/QuanserOptions.hpp>
#include <Mahi/Daq/Io.hpp>

namespace mahi {
namespace daq {

// Forward declarations
class QuanserAI;
class QuanserAO;
class QuanserDI;
class QuanserDO;
class QuanserEncoder;
class QuanserPwm;
class QuanserOtherInput;
class QuanserOtherOutput;

/// Base class from which all Quanser DAQs (Q8Usb, Q2Usb, QPid) derive
class QuanserDaq : public Daq {
public:
    /// Constructor
    QuanserDaq(const char* card_type);
    /// Destructor
    virtual ~QuanserDaq();      
    /// Overrides read_all to use a more efficient Quanser API call
    virtual bool read_all() override;
    /// Override write_all to use a more efficient Quanser API call
    virtual bool write_all() override;
    /// Set Quanser DAQ specific options
    bool set_options(const QuanserOptions& options);
    /// Set Quanser DAQ specific options
    QuanserOptions get_options() const;
    /// Is the DAQ currently valid?
    bool valid() const;
    /// Returns the DAQ manufacturer name (DAQ must be open)
    std::string manufacturer() const;
    /// Returns the DAQ product name (DAQ must be open)
    std::string product_name() const;
    /// Returns the DAQ model name (DAQ must be open)
    std::string model_name() const;
    /// Returns the DAQ serial number (DAQ must be open)
    std::string serial_number() const;
    /// Returns the HIL SDK version
    static std::string hil_version();
protected:
    /// Provides QuanserDaq with your modules so that it can perform more efficient read operations.
    /// Pass nullptr for those that your DAQ does not have. If this isn't called, then the default
    /// Daq implementation is use (i.e. Module recursion)
    void config_read(QuanserAI* AI, QuanserDI* DI, QuanserEncoder* ENC, QuanserOtherInput* OI);
    /// Provides QuanserDaq with your modules so that it can perform more efficient write operations
    /// Pass nullptr for those that your DAQ does not have. If this isn't called, then the default
    /// Daq implementation is use (i.e. Module recursion)
    void config_write(QuanserAO* AO, QuanserDO* DO, QuanserPwm* PWM, QuanserOtherOutput* OO);
protected:
    /// Quanser DAQ open impl
    bool on_daq_open() override;
    /// Quanser DAQ close impl
    bool on_daq_close() override;
protected:
    /// Quanser card type string
    const char* m_card_type;
    /// Quanser integer ID
    const int m_id;
    /// Quanser card handle, valid after open is called
    QuanserHandle m_h;
    /// The current options
    QuanserOptions m_options;
private:
    /// PIMPL idiom for implementing synced read/write operations
    struct ReadWriteImpl;
    std::unique_ptr<ReadWriteImpl> m_rw;
};

} // namespace daq
} // namespace mahi