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
    Fused<DIModule,QuanserDaq>(d,allowed), m_h(h), m_bidirectional(bidirectional)
{
    // Quanser uses char type as their buffer
    set_name(d.name() + ".DI");
    auto on_read_impl = [this](const ChanNum* chs, Logic* vals, std::size_t n) {
        t_error result = hil_read_digital(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to read " << name() << " digital inputs " << quanser_msg(result);
            return false;
        }
        return true;
    };
    on_read.connect(on_read_impl);
    // on channels gain
    if (m_bidirectional) {
        auto on_gain_impl = [this](const ChanNums& gain) {
            auto result = hil_set_digital_directions(m_h, &gain[0], static_cast<t_uint32>(gain.size()), nullptr, 0);
            if (result != 0) {
                LOG(Error) << "Failed to set " << name() << " channels [" << gain << "] directions to inputs.";
                return false;
            }
            LOG(Verbose) << "Set " << name() << " channels [" << gain << "] directions to inputs.";
            return true;
        };
        on_gain_channels.connect(on_gain_impl);
    }
}

} // namespace daq 
} // namespace mahi