#pragma once
#include <Mahi/Daq/Io.hpp>
#include <Mahi/Daq/Quanser2/QuanserHandle.hpp>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>

namespace mahi {
namespace daq {

class QuanserDaq;

class QuanserOtherInput : public ChannelsModule, public Fused<ReadBuffer<double>,QuanserDaq> {
public:
    QuanserOtherInput(QuanserDaq& d, QuanserHandle& h);
protected:
    QuanserHandle m_h;
};

class QuanserOtherOutput : public ChannelsModule, public Fused<WriteBuffer<double>,QuanserDaq> {
public:
    QuanserOtherOutput(QuanserDaq& d, QuanserHandle& h);
protected:
    QuanserHandle m_h;
};

} // namespace daq
} // namespace mahi