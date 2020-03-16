#include <Mahi/Daq/Sensoray/S826Encoder.hpp>
#include <Mahi/Daq/Sensoray/S826.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <windows.h>
#include <826api.h> 
#include <bitset>
#include "SensorayUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

S826Encoder::S826Encoder(S826& s826, unsigned int board) :
    EncoderModule(s826, {0,1,2,3,4,5}),
    timestamps(*this, util::Time::Zero),
    m_board(board)
{
    // set name
    set_name(s826.name() + ".encoder");
    // read impl
    auto read_impl = [this](const ChanNum* chs, Counts* valus, std::size_t n) {
        bool success = true;
        for (int i = 0; i < n; ++i) {
            unsigned int count;
            unsigned int timestamp;
            int result = S826_CounterSnapshot(m_board, chs[i]);
            if (result != S826_ERR_OK) {
                LOG(Error) << "Failed to trigger snapshot on " << name() << " channel number " << chs[i] << ". " << sensoray_msg(result);
                success = false;
            }
            result = S826_CounterSnapshotRead(m_board, chs[i], &count, &timestamp, NULL, 0);
            if (result != S826_ERR_OK) {
                LOG(Error) << "Failed to update " << name() << " channel number " << chs[i] << ". " << sensoray_msg(result);
                success = false;
            }
            timestamps.buffer(chs[i]) = util::microseconds(timestamp);
        }
        return success;
    };
    on_read.connect(read_impl);
    // write impl
    auto write_impl = [this](const ChanNum* chs, const Counts* vals, std::size_t n) {
        bool success = true;
        for (int i = 0; i < n; ++i) {
            unsigned int ucount = (unsigned int)vals[i];
            int result = S826_CounterPreloadWrite(m_board, chs[i], 0, ucount);
            if (result != S826_ERR_OK) {
                LOG(Error) << "Failed to preload counts in to Preload0 register. " << sensoray_msg(result);
                success = false;
            }
            result = S826_CounterPreload(m_board, chs[i], 1, 0);
            if (result != S826_ERR_OK) {
                LOG(Error) << "Failed to reset counts of " << name() << " channel number " << chs[i] << ". " << sensoray_msg(result);
                success = false;
            }
        }
        return success;
    };
    on_write.connect(write_impl);    
    // quadratures write impl
    auto quad_write_impl = [this](const ChanNum* chs, const QuadMode* vals, std::size_t n) {
        bool success = true;
        for (int i = 0; i < n; ++i) {
            uint mode;
            if (vals[i] == QuadMode::X1) 
                mode = S826_CM_K_QUADX1;            
            else if (vals[i] == QuadMode::X2) 
                mode = S826_CM_K_QUADX2;            
            else if (vals[i] == QuadMode::X4) 
                mode = S826_CM_K_QUADX4;        
            else {
                LOG(Error) << "Unsupported quadrature factor (X" << vals[i] << ") requested of " << name();
                success = false;
                continue;
            }
            int result = S826_CounterModeWrite(m_board, chs[i], mode);
            if (result != S826_ERR_OK) {
                LOG(Error) << "Failed to set quadrature mode of " << name() << " channel number " << chs[i] << ". " << sensoray_msg(result);
                success = false;
            }
        }
        return success;
    };
    quadratures.on_write.connect(quad_write_impl);
    // on channels gained
    auto on_gain_impl = [this](const ChanNums& chs) {
        bool success = quadratures.write(chs, std::vector<QuadMode>(chs.size(), QuadMode::X4));
        for (auto& ch : chs) {
            int result = S826_CounterStateWrite(m_board, ch, 1);
            if (result != S826_ERR_OK) {
                LOG(Error) << "Failed to write counter state of " << name() << " channel number " << ch << ". " << sensoray_msg(result);
                success = false;
            }
        }
        return success;
    };
    on_gain_channels.connect(on_gain_impl);
    // set channel numbers
    set_channels({0,1,2,3,4,5});
}

} // namespace daq
} // namespace mahi
