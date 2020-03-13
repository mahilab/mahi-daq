#include "SensorayUtils.hpp"
#include <windows.h>
#include <826api.h> 

namespace mahi {
namespace daq {

std::string sensoray_msg(int error) {
    switch(error) {
        case S826_ERR_OK           : return "No error";
        case S826_ERR_BOARD        : return "Illegal board number";
        case S826_ERR_VALUE        : return "Illegal argument value";
        case S826_ERR_NOTREADY     : return "Device not ready or timeout waiting for device";
        case S826_ERR_CANCELLED    : return "Wait cancelled";
        case S826_ERR_DRIVER       : return "Driver call failed";
        case S826_ERR_MISSEDTRIG   : return "Missed adc trigger";
        case S826_ERR_DUPADDR      : return "Two boards set to same board number";
        case S826_ERR_BOARDCLOSED  : return "Board is not open";
        case S826_ERR_CREATEMUTEX  : return "Can't create mutex";
        case S826_ERR_MEMORYMAP    : return "Can't map board to memory address";
        case S826_ERR_MALLOC       : return "Can't allocate memory";
        case S826_ERR_FIFOOVERFLOW : return "Counter's snapshot fifo overflowed";
        case S826_ERR_LOCALBUS     : return "Can't read local bus (register contains illegal value)";
        case S826_ERR_OSSPECIFIC   : return "Port-specific error (base error number)";
        default                    : return "Unknown error code";
    }
}

} // namespace 
} // namespace 