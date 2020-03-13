#include <Mahi/Daq/Sensoray/S826.hpp>
#include <windows.h>
#include <826api.h> 
#include <bitset>
#include <Mahi/Util/Logging/Log.hpp>
#include "SensorayUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {
    
S826::S826(int board) :
    m_board(board)
{
    // set name
    set_name("s826_" + std::to_string(board));
    // open
    open();
}

S826::~S826() {
    if (is_enabled())
        disable();
    if (is_open())
        close();
}

bool S826::on_daq_open() {
    // open comms with all boards
    bool success = true;
    int detected_boards = S826_SystemOpen();
    if (detected_boards == S826_ERR_DUPADDR) {
        LOG(Error) << "More than one S826 board with same board number detected.";
        success = false;
    }
    if (detected_boards == 0) {
        LOG(Error) << "No S826 boards detected.";
        success = false;
    }
    std::bitset<32> m_boardbits(detected_boards);
    if (m_boardbits[m_board]) {
        // call on_open for modules
        // if (!AI.on_open())
        //     success = false;
        // if (!AO.on_open())
        //     success = false;
        // if (!encoder.on_open())
        //     success = false;
        return true;
    }
    else {
        LOG(Error) << "The requested S826 board " << m_board << " was not detected.";
        success = false;
    }
    return success;
}

bool S826::on_daq_close() {
    S826_SystemClose();
    return true;
}

bool S826::on_daq_enable() {
    return true;
}

bool S826::on_daq_disable() {
    return true;
}

util::Time S826::time() const {
    unsigned int timestamp;
    int result = S826_TimestampRead(m_board, &timestamp);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to read " << name() << " timestamp (" << sensoray_msg(result) << ").";
        return Time::Zero;
    }
    return util::microseconds(timestamp);
}

} // namespace daq
} // namespace mahi
