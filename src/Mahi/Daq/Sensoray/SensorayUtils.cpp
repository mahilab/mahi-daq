#include "SensorayUtils.hpp"
#include <windows.h>
#include <826api.h> 

namespace mahi {
namespace daq {

std::string sensoray_msg(int error) {
    std::string msg;
    switch(error) {
        case S826_ERR_OK           : msg = "No error";
        case S826_ERR_BOARD        : msg = "Illegal board number";
        case S826_ERR_VALUE        : msg = "Illegal argument value";
        case S826_ERR_NOTREADY     : msg = "Device not ready or timeout waiting for device";
        case S826_ERR_CANCELLED    : msg = "Wait cancelled";
        case S826_ERR_DRIVER       : msg = "Driver call failed";
        case S826_ERR_MISSEDTRIG   : msg = "Missed adc trigger";
        case S826_ERR_DUPADDR      : msg = "Two boards set to same board number";
        case S826_ERR_BOARDCLOSED  : msg = "Board is not open";
        case S826_ERR_CREATEMUTEX  : msg = "Can't create mutex";
        case S826_ERR_MEMORYMAP    : msg = "Can't map board to memory address";
        case S826_ERR_MALLOC       : msg = "Can't allocate memory";
        case S826_ERR_FIFOOVERFLOW : msg = "Counter's snapshot fifo overflowed";
        case S826_ERR_LOCALBUS     : msg = "Can't read local bus (register contains illegal value)";
        case S826_ERR_OSSPECIFIC   : msg = "Port-specific error (base error number)";
        default                    : msg = "Unknown error code";
    }
    return "(Sensoray Error #" + std::to_string(error) + ": " + msg + ")";
}

} // namespace 
} // namespace 