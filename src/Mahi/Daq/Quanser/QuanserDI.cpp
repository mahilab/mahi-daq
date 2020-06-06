#include <Mahi/Daq/Quanser/QuanserDI.hpp>
#include <Mahi/Daq/Quanser/QuanserDaq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>
#include <Mahi/Util/Print.hpp>
#include "QuanserUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserDI::QuanserDI(QuanserDaq& d,QuanserHandle& h, bool bidirectional, const ChanNums& allowed) : 
    DIModule(d,allowed), m_h(h), m_bidirectional(bidirectional)
{
    // Quanser uses char type as their buffer
    set_name(d.name() + ".DI");
    auto on_read_impl = [this](const ChanNum* chs, TTL* vals, std::size_t n) {
        t_error result = hil_read_digital(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to read " << name() << " digital inputs " << quanser_msg(result);
            return false;
        }
        return true;
    };
    connect_read(*this, on_read_impl);
}

bool QuanserDI::init_channels(const ChanNums& chs) {
    if (chs.size() == 0)
        return true;
    if (!m_bidirectional)
        return true;
    auto result = hil_set_digital_directions(m_h, &chs[0], static_cast<t_uint32>(chs.size()), nullptr, 0);
    if (result != 0) {
        LOG(Error) << "Failed to set " << name() << " channels " << chs << " directions to inputs.";
        return false;
    }
    LOG(Verbose) << "Set " << name() << " channels " << chs << " directions to inputs.";
    return true;
}

bool QuanserDI::on_daq_open() {
    return init_channels(channels());
}

bool QuanserDI::on_gain_channels(const ChanNums& chs) {
    if (!daq().is_open())
        return true;
    return init_channels(chs);
}


} // namespace daq 
} // namespace mahi