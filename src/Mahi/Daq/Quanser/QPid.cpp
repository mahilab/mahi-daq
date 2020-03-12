#include <Mahi/Daq/Quanser/QPid.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>

using namespace mahi::util;

namespace mahi {
namespace daq {

QPid::QPid() :
    QuanserDaq("qpid_e"),
    AI(*this, m_h, {0,1,2,3,4,5,6,7}), 
    AO(*this, m_h, {0,1,2,3,4,5,6,7}),
    DI(*this, m_h, true, 
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
         10,11,12,13,14,15,16,17,18,19,
         20,21,22,23,24,25,26,27,28,29,
         30,31,32,33,34,35,36,37,38,39,
         40,41,42,43,44,45,46,47,48,49,
         50,51,52,53,54,55,56,57,58,59}), 
    DO(*this, m_h, true, 
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
         10,11,12,13,14,15,16,17,18,19,
         20,21,22,23,24,25,26,27,28,29,
         30,31,32,33,34,35,36,37,38,39,
         40,41,42,43,44,45,46,47,48,49,
         50,51,52,53,54,55}),
    PWM(*this, m_h, {0,1,2,3,4,5,6,7}),
    encoder(*this, m_h, {0,1,2,3,4,5,6,7}), 
    velocity(*this, m_h, encoder, {0,1,2,3,4,5,6,7}),
    watchdog(*this, m_h, 100_ms)
{   
    // open
    open();
    // establish shared pins relationships
    ChanneledModule::ShareList list(56);
    for (ChanNum i = 0; i < 56; ++i)
        list[i] = {{i},{i}};
    DI.share_pins_with(&DO,list);
    // set the initial channels
    AI.set_channels({0,1,2,3,4,5,6,7}); 
    AO.set_channels({0,1,2,3,4,5,6,7});
    DI.set_channels({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                     10,11,12,13,14,15,16,17,18,19,
                     20,21,22,23,24,25,26,27,28,29,
                     30,31,32,33,34,35,36,37,38,39,
                     40,41,42,43,44,45,46,47,48,49,
                     50,51,52,53,54,55}); 
    PWM.set_channels({0,1,2,3,4,5,6,7});
    encoder.set_channels({0,1,2,3,4,5,6,7}); 
    velocity.set_channels({0,1,2,3,4,5,6,7});
    // configure synced reads
    config_read(&AI, &DI, &encoder, &velocity);
    // configure synced writes
    config_write(&AO, &DO, &PWM, nullptr);
}

QPid::~QPid() {
    if (is_enabled())
        disable();
    if (is_open()) {
        set_options(QuanserOptions());
        close();
    }
}

} // namespace daq
} // namespace mahi