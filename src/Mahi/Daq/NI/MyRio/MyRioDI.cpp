#include "Detail/MyRioFpga60/MyRio.h"
#include <Mahi/Daq/NI/MyRio/MyRioConnector.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioDI.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include "MyRioUtils.hpp"

using namespace mahi::util;

extern NiFpga_Session myrio_session;

namespace mahi {
namespace daq {

MyRioDI::MyRioDI(MyRioConnector& connector, const ChanNums& allowed) :
    DIModule(connector, allowed),
    m_conn(connector)
{
    // set name
    set_name(m_conn.name() + ".DI");
    // read impl
    auto read_impl = [this](const ChanNum* chs, Logic* vals, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) 
            vals[i] = get_bit(INS[m_conn.type][chs[i] / 8], chs[i] % 8) ? HIGH : LOW;
        return true;
    };
    connect_read(*this, read_impl);
    // on channels gained impl   
    auto gain_impl = [this](const ChanNums& chs) {
        auto ss = SYSSELECT[m_conn.type];
        for (auto& c : chs) {
            // clear SYSSELECT registers
            if (m_conn.type == MyRioConnector::MxpA || m_conn.type == MyRioConnector::MxpB) {
                switch(c) {
                    case 5:  
                    case 6:
                    case 7:  clr_bit(ss, 0); 
                             clr_bit(ss, 1); break; // disables SPI[0] 
                    case 8:  clr_bit(ss, 2); break; // disables PWM[0]
                    case 9:  clr_bit(ss, 3); break; // disables PWM[1]
                    case 10: clr_bit(ss, 4); break; // disables PWM[2]
                    case 11:  
                    case 12: clr_bit(ss, 5); break; // disables ENC[0]
                    case 14:  
                    case 15: clr_bit(ss, 7); break; // disables I2C[0]
                    default: break;
                }
            }
            else if (m_conn.type == MyRioConnector::MspC) {
                switch(c) {
                    case 0:
                    case 2: clr_bit(ss, 0); break; // disables ENC[0]
                    case 4:
                    case 6: clr_bit(ss, 2); break; // disables ENC[1]
                    case 3: clr_bit(ss, 1); break; // disables PWM[0]
                    case 7: clr_bit(ss, 3); break; // disables PWM[1]
                    default: break;
                }
            }
            // set direction as in
            clr_bit(DIRS[m_conn.type][c / 8], c % 8);
        }
        return true;
    };
    on_gain_channels.connect(gain_impl);
}

} // namespace daq
} // namespace mahi
