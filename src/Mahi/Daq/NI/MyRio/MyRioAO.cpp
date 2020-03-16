#include "Detail/MyRioFpga60/MyRio.h"
#include <Mahi/Daq/NI/MyRio/MyRioConnector.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioAO.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include "MyRioUtils.hpp"

using namespace mahi::util;

extern NiFpga_Session myrio_session;

namespace mahi {
namespace daq {

MyRioAO::MyRioAO(MyRioConnector& connector, const ChanNums& allowed) :
    AOModule(connector, allowed),
    m_conn(connector)
{
    set_name(m_conn.name() + ".AO");
    auto write_impl = [this](const ChanNum* chs, const Voltage* vals, std::size_t n) {
        bool success = true;
        NiFpga_Status status;
        uint16_t valueScaled;
        for (std::size_t i = 0; i < n; ++i) {
            double value = vals[i];
            if (m_conn.type == MyRioConnector::Type::MspC) {
                value = (value - AO_OFFSETS[m_conn.type][chs[i]]) / AO_WEIGHTS[m_conn.type][chs[i]];
                value = (value < INT16_MIN) ? INT16_MIN : value;
                value = (value > INT16_MAX) ? INT16_MAX : value;
                value += (value < 0.0) ? -0.5 : 0.5;
                valueScaled = (uint16_t)((int16_t)(value));
            }
            else {
                value = (value - AO_OFFSETS[m_conn.type][chs[i]]) / AO_WEIGHTS[m_conn.type][chs[i]] + 0.5;
                value = (value < 0) ? 0 : value;
                value = (value > UINT16_MAX) ? UINT16_MAX : value;
                valueScaled = (uint16_t) value;
            }
            status = NiFpga_WriteU16(myrio_session, AO_REGISTERS[m_conn.type][chs[i]], valueScaled);
            if (status < 0) {
                LOG(Error) << "Failed to write " << name() << " channel number " << chs[i];
                success = false;
            }
            status = NiFpga_WriteU16(myrio_session, AOSYSGO, 1);
            if (status < 0) {
                LOG(Error) << "Failed to write " << name() << " channel number " << chs[i];
                success = false;
            }
        }
        return success;
    };
    on_write.connect(write_impl);
}

} // namespace daq
} // namespace mahi
