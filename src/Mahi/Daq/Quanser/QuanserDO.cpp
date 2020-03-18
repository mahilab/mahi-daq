#include <Mahi/Daq/Quanser/QuanserDO.hpp>
#include <Mahi/Daq/Quanser/QuanserDaq.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"
#include <Mahi/Util/Logging/Log.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserDO::QuanserDO(QuanserDaq& d, QuanserHandle& h, bool bidirectional, const ChanNums& allowed)  : 
    DOModule(d,allowed),
    expire_values(*this, 0), m_h(h), m_bidirectional(bidirectional)
{
    set_name(d.name() + ".DO");
    /// Write Channels
    auto write_impl = [this](const ChanNum *chs, const Logic *vals, std::size_t n) {
        t_error result = hil_write_digital(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result != 0) {
            LOG(Error) << "Failed to write " << this->name() << " digital outputs " << quanser_msg(result);
            return false;
        }
        return true;
    };
    connect_write(*this, write_impl);
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
    connect_write(expire_values, expire_write_impl);
    // on channels gained
    auto on_gain_impl = [this](const ChanNums& gain) {
        if (m_bidirectional) {
            auto result = hil_set_digital_directions(m_h, nullptr, 0, &gain[0], static_cast<t_uint32>(gain.size()));
            if (result != 0) {
                LOG(Error) << "Failed to set " << name() << " channels " << gain << " directions to outputs.";
                return false;
            }
        }
        LOG(Verbose) << "Set " << name() << " channels " << gain << " directions to outputs.";
        return expire_values.write(std::vector<Logic>(channels().size(), LOW));
    };
    on_gain_channels.connect(on_gain_impl);
}

} // namespace daq 
} // namespace mahi