#include <Mahi/Daq/Sensoray/S826Watchdog.hpp>
#include <Mahi/Daq/Sensoray/S826.hpp>
#include <windows.h>
#include <826api.h> 
#include <bitset>
#include "SensorayUtils.hpp"

#include <Mahi/Util/Logging/Log.hpp>
using namespace mahi::util;

namespace mahi {
namespace daq {

S826Watchdog::S826Watchdog(S826& s826, unsigned int board, util::Time timeout) : 
    Watchdog(s826), m_board(board)
{
    set_name(s826.name() + ".watchdog");
}

bool S826Watchdog::start() {
    unsigned int microseconds = (unsigned int)m_timout.as_microseconds();
    unsigned int wdtiming[5] = {microseconds * 50, 1, 1, 0, 0};
    int result;
    result = S826_SafeWrenWrite(m_board, S826_SAFEN_SWE);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to enable safe write on " << name() << ". " << sensoray_msg(result);
        return false;
    }
    result = S826_WatchdogConfigWrite(0, 0x10, wdtiming); // activate safemode upon Timer0 event.
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to configure watchdog on " << name() << ". " << sensoray_msg(result);
        return false;
    }
    result = S826_WatchdogEnableWrite(m_board, 1);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to start watchdog on " << name() << ". " << sensoray_msg(result);
        return false;
    }
    else {
        LOG(Verbose) << "Started watchdog on " << name();
        return true;
    }
}

bool S826Watchdog::kick() {
    int result = S826_WatchdogKick(m_board, 0x5A55AA5A);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to kick watchdog on " << name() << ". " << sensoray_msg(result);
        return false;
    }
    return true;
}

bool S826Watchdog::stop() {
    int result = S826_WatchdogEnableWrite(m_board, 0);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to stop watchdog on " << name() << ". " << sensoray_msg(result);
        return false;
    }
    else {
        LOG(Verbose) << "Stopped watchdog on " << name();
        return true;
    }
}

bool S826Watchdog::is_expired() {
    if (S826_WatchdogEventWait(m_board, 0) == S826_ERR_OK)
        return true;
    return false;
}

bool S826Watchdog::clear() {
    int result;    
    result = S826_SafeWrenWrite(m_board, S826_SAFEN_SWE);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to enable safe write on " << name() << ". " << sensoray_msg(result);
        return false;
    }
    result = S826_SafeControlWrite(m_board, 0, 0);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to stop clear watchdog on " << name() << ". " << sensoray_msg(result);
        return false;
    }
    return true;
}

} // namespace daq
} // namespace mahi