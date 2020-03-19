#include "Detail/MyRioFpga60/MyRio.h"
#include <Mahi/Daq/NI/MyRio/MyRio.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioConnector.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioLed.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include "MyRioUtils.hpp"

using namespace mahi::util;

extern NiFpga_Session myrio_session;

namespace mahi {
namespace daq {

MyRioLed::MyRioLed(MyRio& myrio, const ChanNums& allowed) :
    DOModule(myrio, allowed)
{
    set_name(myrio.name() + ".LED");
    // write impl
    auto write_impl = [this](const ChanNum* chs, const TTL* vals, std::size_t n) {
        bool success = true;
        for (std::size_t i = 0; i < n; ++i) {
            if (vals[i] == TTL_HIGH)
                set_bit(DOLED30, chs[i]);
            else if (vals[i] == TTL_LOW) 
                clr_bit(DOLED30, chs[i]);
        }
        return success;
    };
    on_write.connect(write_impl);
}
    
} // namespace daq
} // namespace mahi