#include <Mahi/Daq/Quanser2/QuanserAO.hpp>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"
#include <Mahi/Util/Logging/Log.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserAO::QuanserAO(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed)  : 
    Fused<AOModule,QuanserDaq>(d,allowed),
    expire_values(*this, 0), min_values(*this, -10), max_values(*this, 10), m_h(h)
{
    set_name(d.name() + ".AO");
    /// Write Channels
    auto on_write_impl = [this](const ChanNum *chs, const Voltage *vals, std::size_t n) {
        t_error result = hil_write_analog(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result != 0) {
            LOG(Error) << "Failed to write " << this->name() << " analog outputs " << get_quanser_error_message(result);
            return false;
        }
        return true;
    };
    on_write.connect(on_write_impl);
    // Write Expire States
    auto expire_write_impl = [this](const ChanNum* chs, const Voltage* vals, std::size_t n) { 
        t_error result = hil_watchdog_set_analog_expiration_state(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0) {
            LOG(Verbose) << "Set " << name() << " expire analog expiration states";
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << name() << " analog expiration states " << get_quanser_error_message(result);
            return false;
        }        
    };
    expire_values.on_write.connect(expire_write_impl);
    // Write Min Values
    auto min_write_impl = [this](const ChanNum* chs, const Voltage* vals, std::size_t n) { 
        std::vector<Voltage> temp_max(n);
        for (int i = 0; i < n; ++i)
            temp_max[i] = max_values[chs[i]];
        t_error result = hil_set_analog_output_ranges(m_h, chs, static_cast<t_uint32>(n), vals, &temp_max[0]);
        if (result == 0) {
            LOG(Verbose) << "Set " << name() << " analog output ranges"; 
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << name() << " analog output ranges " << get_quanser_error_message(result);
            return false;
        }
    };
    min_values.on_write.connect(min_write_impl);
    // Write Min Values
    auto max_write_impl = [this](const ChanNum* chs, const Voltage* vals, std::size_t n) { 
        std::vector<Voltage> temp_min(n);
        for (int i = 0; i < n; ++i)
            temp_min[i] = min_values[chs[i]];
        t_error result = hil_set_analog_output_ranges(m_h, chs, static_cast<t_uint32>(n), &temp_min[0], vals);
        if (result == 0) {
            LOG(Verbose) << "Set " << name() << " analog output ranges"; 
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << name() << " analog output ranges " << get_quanser_error_message(result);
            return false;
        }
    };
    max_values.on_write.connect(max_write_impl);
}

bool QuanserAO::on_daq_open() {
    for (auto& ch : channels())
        expire_values[ch] = 0.0;
    return expire_values.write(); // may need a sleep here
}

} // namespace daq 
} // namespace mahi