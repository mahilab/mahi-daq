#include <Mahi/Daq/Quanser/QuanserAI.hpp>
#include <Mahi/Daq/Quanser/QuanserDaq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserAI::QuanserAI(QuanserDaq& d,QuanserHandle& h, const ChanNums& allowed) : 
    Fused<AIModule,QuanserDaq>(d, allowed),
    ranges(*this, {-10,10}), m_h(h)
{
    set_name(d.name() + ".AI");
    
    auto on_read_impl = [this](const ChanNum* chs, Voltage* vals, std::size_t n) {
        t_error result = hil_read_analog(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to read " << name() << " analog inputs " << quanser_msg(result);
            return false;
        }
        return true;
    };
    on_read.connect(on_read_impl);
    // Write Ranges
    auto ranges_write_impl = [this](const ChanNum* chs, const Range<Voltage>* vals, std::size_t n) { 
        std::vector<Voltage> temp_mins(n);
        std::vector<Voltage> temp_maxs(n);
        for (int i = 0; i < n; ++i) {
            temp_mins[i] = vals[i].min_val;
            temp_maxs[i] = vals[i].max_val;
        }
        t_error result = hil_set_analog_input_ranges(m_h, chs, static_cast<t_uint32>(n), &temp_mins[0], &temp_maxs[0]);
        if (result == 0) {
            LOG(Verbose) << "Wrote " << name() << " analog input ranges.";
            return true;
        }
        else {
            LOG(Error) << "Failed to write " << name() << " analog input ranges " << quanser_msg(result);
            return false;
        }
    };
    ranges.on_write.connect(ranges_write_impl);
}

} // namespace daq 
} // namespace mahi