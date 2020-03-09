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
#include <Mahi/Daq/DaqBase.hpp>
#include <Mahi/Daq/Quanser2/QuanserHandle.hpp>
#include <Mahi/Daq/Quanser2/QuanserOptions.hpp>
#include <Mahi/Daq/Io.hpp>

namespace mahi {
namespace daq {

class QuanserAI;
class QuanserAO;
class QuanserDI;
class QuanserDO;
class QuanserEncoder;
class QuanserPwm;
class QuanserOtherInput;
class QuanserOtherOutput;

class QuanserDaq : public DaqBase {
public:

    QuanserDaq(const char* card_type);
    ~QuanserDaq();  

    virtual bool read_all() override;
    virtual bool write_all() override;

    /// Set Quanser DAQ specific options
    bool set_options(const QuanserOptions& options);
    /// Set Quanser DAQ specific options
    QuanserOptions get_options() const;
    /// Is the DAQ currently valid?
    bool valid() const;
    /// Returns the DAQ manufactuer name
    std::string manufactuer() const;
    /// Returns the DAQ product name
    std::string product_name() const;
    /// Returns the DAQ model name
    std::string model_name() const;
    /// Returns the HIL SDK version
    static std::string hil_version();
protected:
    /// Provides QuanserDaq with your modules so that it can perform more efficient read operations.
    /// Pass nullptr for those that your DAQ does not have.
    void config_read(QuanserAI* AI, QuanserDI* DI, QuanserEncoder* enc, QuanserOtherInput* other);
    /// Provides QuanserDaq with your modules so that it can perform more efficient write operations
    /// Pass nullptr for those that your DAQ does not have.
    void config_write(QuanserAO* AO, QuanserDO* DO, QuanserPwm* pwm, QuanserOtherOutput* other);

protected:

    bool on_open() override;
    bool on_close() override;
    bool on_enable() override;
    bool on_disable() override;

    const char* m_card_type;
    int m_id;
    QuanserHandle m_handle;
    QuanserOptions m_options;

private:
    QuanserAI* m_AI = nullptr;
    QuanserAO* m_AO = nullptr;
    QuanserDI* m_DI = nullptr;
    QuanserDO* m_DO = nullptr;
    QuanserEncoder* m_enc = nullptr;
    QuanserPwm* m_pwm = nullptr;   
    QuanserOtherInput*  m_other_read = nullptr;
    QuanserOtherOutput* m_other_write = nullptr;
    std::size_t m_other_in_off = 0, m_other_out_off = 0;
};

} // namespace daq
} // namespace mahi