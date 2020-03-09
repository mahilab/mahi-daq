#include <Mahi/Daq/Quanser2/QuanserOther.hpp>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserOtherInput::QuanserOtherInput(QuanserDaq& d, QuanserHandle& h) : 
    Module(d),
    Fused<ReadBuffer<double>,QuanserDaq>(*this, 0), 
    m_h(h)
{ 
    set_name(d.get_name() + ".OI");
    auto on_read_impl = [this](const ChanNum* chs, double* vals, std::size_t n) {
        for (int i = 0; i < n; ++i)
            LOG(Warning) << "CH: " << chs[i];
        t_error result = hil_read_other(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to read " << get_name() << " " << get_quanser_error_message(result);
            return false;
        }
        return true;
    };
    on_read.connect(on_read_impl); 
}


QuanserOtherOutput::QuanserOtherOutput(QuanserDaq& d, QuanserHandle& h) : 
    Module(d),
    Fused<WriteBuffer<double>,QuanserDaq>(*this, 0), 
    m_h(h)
{ 
    set_name(d.get_name() + ".OO");
    auto on_read_impl = [this](const ChanNum* chs, const double* vals, std::size_t n) {
        t_error result = hil_write_other(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to write " << get_name() << " " << get_quanser_error_message(result);
            return false;
        }
        return true;
    };
    on_write.connect(on_read_impl); 
}

} // namespace daq
} // namespace mahi