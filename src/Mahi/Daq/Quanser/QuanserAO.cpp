#include <Mahi/Daq/Quanser/QuanserAO.hpp>
#include <Mahi/Daq/Quanser/QuanserDaq.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"
#include <Mahi/Util/Logging/Log.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserAO::QuanserAO(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed)  : 
    Fused<AOModule,QuanserDaq>(d,allowed),
    expire_values(*this, 0), ranges(*this, {-10,10}), m_h(h)
{
    set_name(d.name() + ".AO");
    /// Write Channels
    auto on_write_impl = [this](const ChanNum *chs, const Voltage *vals, std::size_t n) {
        t_error result = hil_write_analog(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result != 0) {
            LOG(Error) << "Failed to write " << this->name() << " analog outputs " << quanser_msg(result);
            return false;
        }
        return true;
    };
    on_write.connect(on_write_impl);
    // Write Expire States
    auto expire_write_impl = [this](const ChanNum* chs, const Voltage* vals, std::size_t n) { 
        t_error result = hil_watchdog_set_analog_expiration_state(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0) {
            LOG(Verbose) << "Wrote " << name() << " expire analog expiration states.";
            return true;
        }
        else {
            LOG(Error) << "Failed to write " << name() << " analog expiration states " << quanser_msg(result);
            return false;
        }        
    };
    expire_values.on_write.connect(expire_write_impl);
    // Write Ranges
    auto ranges_write_impl = [this](const ChanNum* chs, const Range<Voltage>* vals, std::size_t n) { 
        std::vector<Voltage> temp_mins(n);
        std::vector<Voltage> temp_maxs(n);
        for (int i = 0; i < n; ++i) {
            temp_mins[i] = vals[i].min_val;
            temp_maxs[i] = vals[i].max_val;
        }
        t_error result = hil_set_analog_output_ranges(m_h, chs, static_cast<t_uint32>(n), &temp_mins[0], &temp_maxs[0]);
        if (result == 0) {
            LOG(Verbose) << "Wrote " << name() << " analog output ranges.";
            return true;
        }
        else {
            LOG(Error) << "Failed to write " << name() << " analog output ranges " << quanser_msg(result);
            return false;
        }
    };
    ranges.on_write.connect(ranges_write_impl);
    // on channels gained
    auto on_gain = [this](const ChanNums& gained) {
        return expire_values.write(gained, std::vector<Voltage>(gained.size(), 0)) && 
               ranges.write(gained, std::vector<Range<Voltage>>(gained.size(), {-10,10}));
    };
    on_gain_channels.connect(on_gain);
}

} // namespace daq 
} // namespace mahi