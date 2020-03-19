#include "Detail/MyRioFpga60/MyRio.h"
#include <Mahi/Daq/NI/MyRio/MyRioConnector.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioAI.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include "MyRioUtils.hpp"

using namespace mahi::util;

extern NiFpga_Session myrio_session;

namespace mahi {
namespace daq {

MyRioAI::MyRioAI(MyRioConnector& connector, const ChanNums& allowed) :
    AIModule(connector, allowed),
    m_conn(connector)
{
    set_name(m_conn.name() + ".AI");
    auto read_impl = [this](const ChanNum* chs, Volts* vals, std::size_t n) {
        bool success = true;
        for (std::size_t i = 0; i < n; ++i) {
            uint16_t value = 0;
            NiFpga_Status status = NiFpga_ReadU16(myrio_session, AI_REGISTERS[m_conn.type][chs[i]], &value);
            if (status < 0) {
                LOG(Error) << "Failed to read " << name() << " channel number "  << chs[i];
                success = false;
            }
            else {
                if (m_conn.type == MyRioConnector::Type::MspC)
                    vals[i] = (int16_t)value * AI_WEIGHTS[m_conn.type][chs[i]] + AI_OFFSETS[m_conn.type][chs[i]];
                else
                    vals[i] = value * AI_WEIGHTS[m_conn.type][chs[i]] + AI_OFFSETS[m_conn.type][chs[i]];
                success = true;
            }
        }
        return success;
    };
    connect_read(*this, read_impl);
}


} // namespace daq
} // namespace mahi
