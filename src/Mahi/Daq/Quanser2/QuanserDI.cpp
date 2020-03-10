#include <Mahi/Daq/Quanser2/QuanserDI.hpp>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>
#include <Mahi/Util/Print.hpp>
#include "QuanserUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserDI::QuanserDI(QuanserDaq& d,QuanserHandle& h, const ChanNums& channel_numbers) : 
    Fused<DIModule,QuanserDaq>(d), m_h(h)
{
    // Quanser uses char type as their buffer
    set_name(d.name() + ".DI");
    set_channel_numbers(channel_numbers);    
    auto on_read_impl = [this](const ChanNum* chs, Logic* vals, std::size_t n) {
        t_error result = hil_read_digital(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to read " << name() << " digital inputs " << get_quanser_error_message(result);
            return false;
        }
        return true;
    };
    on_read.connect(on_read_impl);
}

} // namespace daq 
} // namespace mahi