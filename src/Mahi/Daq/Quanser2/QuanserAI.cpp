#include <Mahi/Daq/Quanser2/QuanserAI.hpp>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserAI::QuanserAI(QuanserDaq& d,QuanserHandle& h, const ChanNums& allowed) : 
    Fused<AIModule,QuanserDaq>(d, allowed),
    min_values(*this, -10), max_values(*this, 10), m_h(h)
{
    set_name(d.name() + ".AI");
    
    auto on_read_impl = [this](const ChanNum* chs, Voltage* vals, std::size_t n) {
        t_error result = hil_read_analog(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to read " << name() << " analog inputs " << get_quanser_error_message(result);
            return false;
        }
        return true;
    };
    on_read.connect(on_read_impl);

    // Write Min Values
    auto min_write_impl = [this](const ChanNum* chs, const Voltage* vals, std::size_t n) { 
        std::vector<Voltage> temp_max(n);
        for (int i = 0; i < n; ++i)
            temp_max[i] = max_values[chs[i]];
        t_error result = hil_set_analog_input_ranges(m_h, chs, static_cast<t_uint32>(n), vals, &temp_max[0]);
        if (result == 0) {
            LOG(Verbose) << "Set " << name() << " analog input ranges"; 
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << name() << " analog input ranges " << get_quanser_error_message(result);
            return false;
        }
    };
    min_values.on_write.connect(min_write_impl);

    // Write Min Values
    auto max_write_impl = [this](const ChanNum* chs, const Voltage* vals, std::size_t n) { 
        std::vector<Voltage> temp_min(n);
        for (int i = 0; i < n; ++i)
            temp_min[i] = min_values[chs[i]];
        t_error result = hil_set_analog_input_ranges(m_h, chs, static_cast<t_uint32>(n), &temp_min[0], vals);
        if (result == 0) {
            LOG(Verbose) << "Set " << name() << " analog input ranges"; 
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << name() << " analog input ranges " << get_quanser_error_message(result);
            return false;
        }
    };
    max_values.on_write.connect(max_write_impl);
}

} // namespace daq 
} // namespace mahi