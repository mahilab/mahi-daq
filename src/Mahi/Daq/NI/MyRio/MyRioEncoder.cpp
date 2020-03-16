#include "Detail/MyRioFpga60/MyRio.h"
#include <Mahi/Daq/NI/MyRio/MyRioConnector.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioEncoder.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include "MyRioUtils.hpp"

using namespace mahi::util;

extern NiFpga_Session myrio_session;

namespace mahi {
namespace daq {

MyRioEncoder::MyRioEncoder(MyRioConnector& connector, const ChanNums& allowed) :
    EncoderModule<MyRioEncoder>(connector, allowed),
    m_conn(connector)
{
    // set name
    set_name(m_conn.name() + ".encoder");
    // read impl
    auto read_impl = [this](const ChanNum* chs, Counts* vals, std::size_t n) {
        bool success = true;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t counts;
            NiFpga_Status status = NiFpga_ReadU32(myrio_session, ENC_CNTR[m_conn.type][chs[i]], &counts);
            if (status < 0) {
                LOG(Error) << "Failed to read counts from encoder register";
                success = false;
            }
            vals[i] = static_cast<int>(counts);
        }
        return success;
    };
    on_read.connect(read_impl);
    // write impl
    auto write_impl = [this](const ChanNum* chs, const Counts* vals, std::size_t n) {
        bool success = true;
        for (std::size_t i = 0; i < n; ++i) {
            if (vals[i] == 0) {
                set_bit(ENC_CNFG[m_conn.type][chs[i]], 1);
                clr_bit(ENC_CNFG[m_conn.type][chs[i]], 1);
                success = true;
            }
            else  {
                LOG(Error) << "myRIO Encoder counts can only be reset to zero";
                success = false;
            }
        };
        return success;
    };
    on_write.connect(write_impl);
    // gain impl
    auto gain_impl = [this](const ChanNums& chs) {
        auto ss = SYSSELECT[m_conn.type];
        for (auto& ch : chs) {
            if (m_conn.type == MyRioConnector::MxpA || m_conn.type == MyRioConnector::MxpB) {
                switch(ch) {
                    case 0: set_bit(ss, 5); break; // disables DIO 11,12
                    default: break;
                }
            }
            else if (m_conn.type == MyRioConnector::MspC) {
                switch(ch) {
                    case 0: set_bit(ss, 0); break; // disables DIO 0,2
                    case 1: set_bit(ss, 2); break; // disables DIO 4,6
                    default: break;
                }
            }
            clr_bit(ENC_CNFG[m_conn.type][ch], 2); // set to quadrature mode
            set_bit(ENC_CNFG[m_conn.type][ch], 0); // enable encoder
        }
        return true;
    };
    on_gain_channels.connect(gain_impl);
    // free impl
    auto free_impl = [this](const ChanNums& chs) {
        for (auto& ch : chs) 
            clr_bit(ENC_CNFG[m_conn.type][ch], 0); // disable encoder
        return true;
    };
    on_free_channels.connect(free_impl);
    // quad write impl
    auto quad_impl = [this](const ChanNum* chs, const QuadMode* vals, std::size_t n) {
        bool success = true;
        for (std::size_t i = 0; i < n; ++i) {
            if (vals[i] == QuadMode::X4) 
                clr_bit(ENC_CNFG[m_conn.type][chs[i]], 2);
            else if (vals[i] == QuadMode::X0) 
                set_bit(ENC_CNFG[m_conn.type][chs[i]], 2);
            else {
                LOG(Error) << "myRIO encoders only support X0 and X4 quadrature modes";
                success = false;
            }
        }
        return success;           
    };
    quadratures.on_write.connect(quad_impl);
}
/*
void MyRioEncoder::sync() {
    // determine which channels are currently enabled on FPGA
    ChanNums chs;
    for (auto& ch : allowed_) {
        if (get_bit(sysselect_, bits_[ch])) {
            chs.push_back(ch);
            clr_bit(cnfg_[ch], 2); // set to quadrature mode
            set_bit(cnfg_[ch], 0); // enable encoder
        }
    }
    // set module channels
    set_channels(chs);
}
*/

} // namespace daq
} // namespace mahi
