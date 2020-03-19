#include <hil.h>

#include <Mahi/Daq/Quanser/Q8Usb.hpp>
#include <Mahi/Util/Logging/Log.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

Q8Usb::Q8Usb() :
    QuanserDaq("q8_usb"),
    AI(*this, m_h, {0, 1, 2, 3, 4, 5, 6, 7}),
    AO(*this, m_h, {0, 1, 2, 3, 4, 5, 6, 7}),
    DI(*this, m_h, false, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}),
    DO(*this, m_h, false, {0, 1, 2, 3, 4, 5, 6, 7}),
    PWM(
        *this, m_h, {0, 1, 2, 3, 4, 5, 6, 7},
        [this](const ChanNums& gain) {
            auto opts = get_options();
            for (auto& g : gain)
                opts.pwmX_en[g] = true;
            return set_options(opts);
        },
        [this](const ChanNums& free) {
            auto opts = get_options();
            for (auto& g : free)
                opts.pwmX_en[g] = false;
            return set_options(opts);
        }),
    encoder(*this, m_h, {0, 1, 2, 3, 4, 5, 6, 7}),
    velocity(*this, m_h, encoder, {0, 1, 2, 3, 4, 5, 6, 7}),
    watchdog(*this, m_h, 100_ms) {
    // open
    open();
    // establish shared pins relationships
    create_shared_pins(&DO, &PWM,
                       {{{0}, {0}},
                        {{1}, {1}},
                        {{2}, {2}},
                        {{3}, {3}},
                        {{4}, {4}},
                        {{5}, {5}},
                        {{6}, {6}},
                        {{7}, {7}}});
    // set the initial channels
    AI.set_channels({0, 1, 2, 3, 4, 5, 6, 7});
    AO.set_channels({0, 1, 2, 3, 4, 5, 6, 7});
    DI.set_channels({0, 1, 2, 3, 4, 5, 6, 7});
    DO.set_channels({0, 1, 2, 3, 4, 5, 6, 7});
    encoder.set_channels({0, 1, 2, 3, 4, 5, 6, 7});
    velocity.set_channels({0, 1, 2, 3, 4, 5, 6, 7});
    // configure synced reads
    config_read(&AI, &DI, &encoder, &velocity);
    // configure synced writes
    config_write(&AO, &DO, &PWM, nullptr);
}

Q8Usb::~Q8Usb() {
    if (is_enabled())
        disable();
    if (is_open()) {
        set_options(QuanserOptions());
        close();
    }
}

}  // namespace daq
}  // namespace mahi