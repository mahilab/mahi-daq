#include <Mahi/Daq/Quanser2/QuanserDO.hpp>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"
#include <Mahi/Util/Logging/Log.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserDO::QuanserDO(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed)  : 
    Fused<DOModule,QuanserDaq>(d,allowed),
    expire_values(*this, 0), m_h(h)
{
    set_name(d.name() + ".DO");
    /// Write Channels
    auto on_write_impl = [this](const ChanNum *chs, const Logic *vals, std::size_t n) {
        t_error result = hil_write_digital(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result != 0) {
            LOG(Error) << "Failed to write " << this->name() << " digital outputs " << quanser_msg(result);
            return false;
        }
        return true;
    };
    on_write.connect(on_write_impl);
    // // Write Expire States
    auto expire_write_impl = [this](const ChanNum* chs, const Logic* vals, std::size_t n) { 
        // convert to Quanser t_digital_state
        std::vector<t_digital_state> converted(n);
        for (int i = 0; i < n; ++i) {
            if (vals[i] == HIGH)
                converted.push_back(DIGITAL_STATE_HIGH);
            else
                converted.push_back(DIGITAL_STATE_LOW);
        }
        t_error result;
        result = hil_watchdog_set_digital_expiration_state(m_h, chs, static_cast<ChanNum>(n), &converted[0]);
        if (result == 0) {
            LOG(Verbose) << "Wrote " << name() << " digital output expiration sates.";
            return true;
        }
        else {
            LOG(Error) << "Failed to write " << name() << " digital output expire states " << quanser_msg(result);
            return false;
        }
    };
    expire_values.on_write.connect(expire_write_impl);
    // on channels gained
    auto on_gain = [this](const ChanNums& gained) {
        return expire_values.write(gained, std::vector<Logic>(gained.size(), LOW));
    };
    on_gain_channels.connect(on_gain);
}

} // namespace daq 
} // namespace mahi