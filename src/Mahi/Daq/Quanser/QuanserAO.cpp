#include <Mahi/Daq/Quanser/QuanserAO.hpp>
#include <Mahi/Daq/Quanser/QuanserDaq.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"
#include <Mahi/Util/Logging/Log.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserAO::QuanserAO(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed)  : 
    AOModule(d,allowed),
    expire_values(*this, 0), ranges(*this, {-10,10}), m_h(h)
{
    set_name(d.name() + ".AO");
    /// Write Channels
    auto write_impl = [this](const ChanNum *chs, const Volts *vals, std::size_t n) {
        t_error result = hil_write_analog(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result != 0) {
            LOG(Error) << "Failed to write " << this->name() << " analog outputs " << quanser_msg(result);
            return false;
        }
        return true;
    };
    connect_write(*this, write_impl);
    // Write Expire States
    auto expire_write_impl = [this](const ChanNum* chs, const Volts* vals, std::size_t n) { 
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
    connect_write(expire_values, expire_write_impl);
    // Write Ranges
    auto ranges_write_impl = [this](const ChanNum* chs, const Range<Volts>* vals, std::size_t n) { 
        std::vector<Volts> temp_mins(n);
        std::vector<Volts> temp_maxs(n);
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
    connect_write(ranges, ranges_write_impl);
}

bool QuanserAO::on_gain_channels(const ChanNums& chs) {
    return expire_values.write(chs, std::vector<Volts>(chs.size(), 0)) &&  ranges.write(chs, std::vector<Range<Volts>>(chs.size(), {-10,10}));
}


} // namespace daq 
} // namespace mahi