#include <Mahi/Daq/Quanser/Q2Usb.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>

using namespace mahi::util;

namespace mahi {
namespace daq {

Q2Usb::Q2Usb() :
    QuanserDaq("q2_usb"),
    AI(*this, m_h, {0,1}), 
    AO(*this, m_h, {0,1}),
    DI(*this, m_h, true, {0,1,2,3,4,5,6,7}), 
    DO(*this, m_h, true, {0,1,2,3,4,5,6,7,8}),
    PWM(*this, m_h, {0,1}),
    encoder(*this, m_h, {0,1}), 
    watchdog(*this, m_h, 100_ms)
{
    // open
    open();
    /// Configure LED for user mode and turn off PWM by default
    QuanserOptions opts;
    opts.led = QuanserOptions::LedMode::User;
    opts.d0 = QuanserOptions::DoMode::Digital;
    opts.d1 = QuanserOptions::DoMode::Digital;
    set_options(opts);
    // established shared pins
    ChannelsModule::share(&DI, &DO, {{{0},{0}},{{1},{1}},{{2},{2}},{{3},{3}},{{4},{4}},{{5},{5}},{{6},{6}},{{7},{7}}});
    ChannelsModule::share(&PWM, &DO, {{{0},{0}},{{1},{1}}});
    ChannelsModule::share(&PWM, &DI, {{{0},{0}},{{1},{1}}});
    // conect PWM gain callback
    auto on_pwm_gain = [this](const ChanNums& gain) {
        auto opts = get_options();
        for (auto& g : gain) {
            if (g == 0)
                opts.d0 = QuanserOptions::DoMode::Pwm;
            else if (g == 1)
                opts.d1 = QuanserOptions::DoMode::Pwm;
            else 
                LOG(Error) << "Channel " << g << " is not a supported PWM channel on the Q2-USB.";
        }
        return set_options(opts);
    };
    PWM.on_gain_channels.connect(on_pwm_gain);
    // connect PWM free callback
    auto on_pwm_free = [this](const ChanNums& gain) {
        auto opts = get_options();
        for (auto& g : gain) {
            if (g == 0)
                opts.d0 = QuanserOptions::DoMode::Digital;
            else if (g == 1)
                opts.d1 = QuanserOptions::DoMode::Digital;
            else 
                LOG(Error) << "Channel " << g << " is not a supported PWM channel on the Q2-USB.";
        }
        return set_options(opts);
    };
    PWM.on_free_channels.connect(on_pwm_free);
    // set the initial channels
    AI.set_channels({0,1}); 
    AO.set_channels({0,1});
    DI.set_channels({0,1,2,3,4,5,6,7}); 
    DO.set_channels({8});
    encoder.set_channels({0,1}); 
    // configure synced reads
    config_read(&AI, &DI, &encoder, nullptr);
    // configure synced writes
    config_write(&AO, &DO, &PWM, nullptr);
}

Q2Usb::~Q2Usb() {
    if (is_enabled())
        disable();
    if (is_open()) {
        set_options(QuanserOptions());
        close();
    }
}

} // namespace daq
} // namespace mahi