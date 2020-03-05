#include <mahi/daq/Watchdog.hpp>

namespace mahi {
namespace daq {

//==============================================================================
// CLASS DEFINTIONS
//==============================================================================

Watchdog::Watchdog(util::Time timeout) :
    timeout_(timeout),
    watching_(false)
{
}

Watchdog::~Watchdog() {

}

void Watchdog::set_timeout(util::Time timeout) {
    timeout_ = timeout;
}

} // namespace daq
} // namespace mahi
