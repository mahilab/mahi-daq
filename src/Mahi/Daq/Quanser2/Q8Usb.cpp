#include <Mahi/Daq/Quanser2/Q8Usb.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>



namespace mahi {
namespace daq {

namespace {
ChanNums EIGHT = {0,1,2,3,4,5,6,7};
}

Q8Usb::Q8Usb() :
    QuanserDaq("q8_usb"),
    AI(*this, m_h, EIGHT), AO(*this, m_h, EIGHT),
    DI(*this, m_h, EIGHT), DO(*this, m_h, EIGHT),
    encoder(*this, m_h, EIGHT), velocity(*this, m_h, encoder, EIGHT),
    watchdog(*this, m_h, util::milliseconds(100))
{
    // set the intial channels
    AI.set_channels(EIGHT); AO.set_channels(EIGHT);
    DI.set_channels(EIGHT); DO.set_channels(EIGHT);
    encoder.set_channels(EIGHT); velocity.set_channels(EIGHT);
    // establish shared pins relationships
    // ...
    // configure synced reads
    config_read(&AI, &DI, &encoder, &velocity);
    // configure synced writes
    config_write(&AO, &DO, nullptr, nullptr);
}

Q8Usb::~Q8Usb() {
    if (is_enabled())
        disable();
    if (is_open()) {
        set_options(QuanserOptions());
        close();
    }
}

} // namespace daq
} // namespace mahi