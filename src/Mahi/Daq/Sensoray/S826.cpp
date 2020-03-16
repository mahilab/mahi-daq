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
    AI(*this, board),
    AO(*this, board),
    encoder(*this, board),
    watchdog(*this, board, 10_ms),
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
        success = true;
    }
    else {
        LOG(Error) << "The requested S826 board " << m_board << " was not detected.";
        success = false;
    }
    return success;
}

bool S826::on_daq_close() {
    int result = S826_SystemClose();
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to close " << name() << ". " << sensoray_msg(result);
        return false;
    }
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
