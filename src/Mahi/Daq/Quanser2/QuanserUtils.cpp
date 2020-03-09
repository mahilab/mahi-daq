#include "QuanserUtils.hpp"
#include <hil.h>
#include <quanser_messages.h>

namespace mahi {
namespace daq {

std::string get_quanser_error_message(int error, bool format)
{
    char message[512];
    msg_get_error_message(NULL, error, message, sizeof(message));
    if (format)
        return "(Quanser Error #" + std::to_string(-error) + ": " + std::string(message) + ")";
    else
        return std::string(message);
}

} // namespace mahi
} // namespace daq