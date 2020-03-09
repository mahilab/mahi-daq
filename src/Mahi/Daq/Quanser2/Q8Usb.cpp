#include <Mahi/Daq/Quanser2/Q8Usb.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>



namespace mahi {
namespace daq {

Q8Usb::Q8Usb() :
    QuanserDaq("q8_usb"),
    AI(*this, m_handle, {0,1,2,3,4,5,6,7}),
    AO(*this, m_handle, {0,1,2,3,4,5,6,7}),
    DI(*this, m_handle, {0,1,2,3,4,5,6,7}),
    encoder(*this, m_handle, {0,1,2,3,4,5,6,7}),
    velocity(*this, m_handle, encoder, {0,1,2,3,4,5,6,7})
{
    config_read(&AI, nullptr, &encoder, &velocity);
    config_write(&AO, nullptr, nullptr, nullptr);
}

} // namespace daq
} // namespace mahi