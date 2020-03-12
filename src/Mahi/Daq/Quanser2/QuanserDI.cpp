#include <Mahi/Daq/Quanser2/QuanserDI.hpp>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>
#include <Mahi/Util/Print.hpp>
#include "QuanserUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserDI::QuanserDI(QuanserDaq& d,QuanserHandle& h, const ChanNums& allowed) : 
    Fused<DIModule,QuanserDaq>(d,allowed), m_h(h)
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
}

} // namespace daq 
} // namespace mahi