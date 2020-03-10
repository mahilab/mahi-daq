#include <Mahi/Daq/Watchdog.hpp>

namespace mahi {
namespace daq {

//==============================================================================
// CLASS DEFINTIONS
//==============================================================================

Watchdog::Watchdog(Daq& daq) :
    Module(daq),
    m_timout(util::Time::Inf),
    m_watching(false)
{ }

Watchdog::~Watchdog() { }

void Watchdog::set_timeout(util::Time timeout) {
    m_timout = timeout;
}

} // namespace daq
} // namespace mahi
