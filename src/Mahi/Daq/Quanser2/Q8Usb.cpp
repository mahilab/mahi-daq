#include <Mahi/Daq/Quanser2/Q8Usb.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>



namespace mahi {
namespace daq {

Q8Usb::Q8Usb() :
    QuanserDaq("q8_usb"),
    AI(*this, m_h, {0,1,2,3,4,5,6,7}),
    AO(*this, m_h, {0,1,2,3,4,5,6,7}),
    DI(*this, m_h, {0,1,2,3,4,5,6,7}),
    DO(*this, m_h, {0,1,2,3,4,5,6,7}),
    encoder(*this, m_h, {0,1,2,3,4,5,6,7}),
    velocity(*this, m_h, encoder, {0,1,2,3,4,5,6,7}),
    watchdog(*this, m_h, util::milliseconds(100))
{
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