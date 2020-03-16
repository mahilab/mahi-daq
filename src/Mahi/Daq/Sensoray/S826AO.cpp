#include <Mahi/Daq/Sensoray/S826AO.hpp>
#include <Mahi/Daq/Sensoray/S826.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <windows.h>
#include <826api.h> 
#include <bitset>
#include "SensorayUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

S826AO::S826AO(S826& s826, unsigned int board) :
    AOModule(s826, {0,1,2,3,4,5,6,7}),
    ranges(*this, {-10,10}),
    m_board(board)
{
    // set name
    set_name(s826.name() + ".AO");

    // write impl
    auto write_impl = [this](const ChanNum* chs, const Voltage* vals, std::size_t n) {
        bool success = true;
        for (int i = 0; i < n; ++i) {
            double volts = vals[i];
            uint setpoint;
            switch(m_ranges[chs[i]]) {
                case S826_DAC_SPAN_0_5: setpoint = (uint)(volts * 0xFFFF / 5); break; // 0 to +5V
                case S826_DAC_SPAN_0_10: setpoint = (uint)(volts * 0xFFFF / 10); break; // 0 to +10V
                case S826_DAC_SPAN_5_5: setpoint = (uint)(volts * 0xFFFF / 10) + 0x8000; break; // -5V to +5V
                case S826_DAC_SPAN_10_10: setpoint = (uint)(volts * 0xFFFF / 20) + 0x8000; break; // -10V to +10V
            }            
            int result = S826_DacDataWrite(m_board, chs[i], setpoint, 0);
            if (result != S826_ERR_OK) {
                LOG(Error) << "Failed to update " << name() << " channel number " << chs[i] << ". " << sensoray_msg(result);
                success = false;
            }
        }
        return success;
    };
    on_write.connect(write_impl);

    // range write impl
    auto range_write_impl = [this](const ChanNum* chs, const Range<Voltage>* vals, std::size_t n) {
        bool success = true;
        for (int i = 0; i < n; ++i) {
            uint range;
            if (vals[i].min_val == -10 && vals[i].max_val == 10)
                range = S826_DAC_SPAN_10_10;
            else if (vals[i].min_val == 0 && vals[i].max_val == 10)
                range = S826_DAC_SPAN_0_10;
            else if (vals[i].min_val == -5 && vals[i].max_val == 5)
                range = S826_DAC_SPAN_5_5;
            else if (vals[i].min_val == 0 && vals[i].max_val == 5)
                range = S826_DAC_SPAN_0_5;
            else {
                LOG(Error) << "Failed to set analog output range on " << name() << ". Valid ranges are (-10,10), (0,10), (-5,5) and (0,5).";
                success = false;
                continue;
            }
            m_ranges[chs[i]] = range;
            auto result = S826_DacRangeWrite(m_board, chs[i], range, 0);
            if (result != S826_ERR_OK) {
                LOG(Error) << "Failed to set " << name() << " channel number " << chs[i] << " output range. " << sensoray_msg(result);
                success = false;
            }
        }
        return success;
    };
    ranges.on_write.connect(range_write_impl);
    // on gain impl
    auto on_gain_impl = [this](const ChanNums& chs) {
        return ranges.write(chs, std::vector<Range<Voltage>>(chs.size(),{-10,10}));
    };
    on_gain_channels.connect(on_gain_impl);
    // set channel numbers
    set_channels({0,1,2,3,4,5,6,7});
}

} // namespace daq
} // namespace mahi
