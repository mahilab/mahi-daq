#pragma once
#include <Mahi/Daq/Io.hpp>
#include <Mahi/Daq/Quanser/QuanserHandle.hpp>
#include <Mahi/Daq/Quanser/QuanserDaq.hpp>

namespace mahi {
namespace daq {

class QuanserDaq;

class QuanserOtherInput : public ChanneledModule, public ReadBuffer<double> {
public:
    QuanserOtherInput(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed);
protected:
    friend QuanserDaq;
    QuanserHandle m_h;
};

class QuanserOtherOutput : public ChanneledModule, public WriteBuffer<double> {
public:
    QuanserOtherOutput(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed);
protected:
    friend QuanserDaq;
    QuanserHandle m_h;
};

} // namespace daq
} // namespace mahi