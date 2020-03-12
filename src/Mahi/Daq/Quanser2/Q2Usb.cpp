#include <Mahi/Daq/Quanser2/Q2Usb.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <hil.h>

using namespace mahi::util;

namespace mahi {
namespace daq {

Q2Usb::Q2Usb() :
    QuanserDaq("q2_usb"),
    AI(*this, m_h, {0,1}), 
    AO(*this, m_h, {0,1}),
    DI(*this, m_h, {0,1,2,3,4,5,6,7}), 
    DO(*this, m_h, {0,1,2,3,4,5,6,7,8}),
    PWM(*this, m_h, {0,1}),
    encoder(*this, m_h, {0,1}), 
    watchdog(*this, m_h, util::milliseconds(100))
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
    // connect DI gain callback
    auto on_di_gain = [this](const ChanNums& gain) {
        auto result = hil_set_digital_directions(m_h, &gain[0], static_cast<t_uint32>(gain.size()), nullptr, 0);
        if (result != 0) {
            LOG(Error) << "Failed to set " << DI.name() << " channels [" << gain << "] directions to inputs.";
            return false;
        }
        LOG(Verbose) << "Set " << DI.name() << " channels [" << gain << "] directions to inputs.";
        return true;
    };
    DI.on_gain_channels.connect(on_di_gain);
    // connect DO gain callback
    auto on_do_gain = [this](const ChanNums& gain) {
        auto result = hil_set_digital_directions(m_h, nullptr, 0, &gain[0], static_cast<t_uint32>(gain.size()));
        if (result != 0) {
            LOG(Error) << "Failed to set " << DO.name() << " channels [" << gain << "] directions to outputs.";
            return false;
        }
        LOG(Verbose) << "Set " << DO.name() << " channels [" << gain << "] directions to outputs.";
        DO.expire_values.write(std::vector<Logic>(DO.channels().size(), LOW));
        return true;
    };
    DO.on_gain_channels.connect(on_do_gain);
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
        set_options(opts);
        return true;
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
        set_options(opts);
        return true;
    };
    PWM.on_free_channels.connect(on_pwm_free);
    // set the intial channels
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