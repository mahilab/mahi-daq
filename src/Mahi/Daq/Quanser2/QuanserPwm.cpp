#include <Mahi/Daq/Quanser2/QuanserPwm.hpp>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <hil.h>
#include "QuanserUtils.hpp"
#include <Mahi/Util/Logging/Log.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserPwm::QuanserPwm(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed)  : 
    Fused<OutputModule<double>,QuanserDaq>(d,allowed),
    modes(*this, Mode::DutyCycle), expire_values(*this, 0), frequencies(*this, 10000), duty_cycles(*this, 0.5), m_h(h)
{
    set_name(d.name() + ".PWM");
    /// Write Channels
    auto on_write_impl = [this](const ChanNum *chs, const double *vals, std::size_t n) {
        t_error result = hil_write_pwm(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result != 0) {
            LOG(Error) << "Failed to write " << this->name() << " PWM outputs. " << quanser_msg(result);
            return false;
        }
        return true;
    };
    on_write.connect(on_write_impl);
    // Write Expire States
    auto expire_write_impl = [this](const ChanNum* chs, const double* vals, std::size_t n) { 
        t_error result = hil_watchdog_set_pwm_expiration_state(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0) {
            LOG(Verbose) << "Set " << name() << " expire PWM expiration states.";
            return true;
        }
        LOG(Error) << "Failed to set " << name() << " PWM expiration states. " << quanser_msg(result);
        return false;
    };
    expire_values.on_write.connect(expire_write_impl);
    // Write modes
    auto mode_write_impl = [this](const ChanNum* chs, const Mode* vals, std::size_t n) {
        std::vector<t_pwm_mode> qmodes(n);
        for (int i = 0; i < n; ++i) {
            if (vals[i] == Mode::DutyCycle)
                qmodes[i] = PWM_DUTY_CYCLE_MODE;
            else if (vals[i] == Mode::Frequency)
                qmodes[i] = PWM_FREQUENCY_MODE;
            else if (vals[i] == Mode::Period)
                qmodes[i] = PWM_PERIOD_MODE;
            else if (vals[i] == Mode::OneShot)
                qmodes[i] = PWM_ONE_SHOT_MODE;
        }
        t_error result = hil_set_pwm_mode(m_h, chs, static_cast<t_uint32>(n), &qmodes[0]);
        if (result == 0) {
            LOG(Verbose) << "Wrote " << name() << " PWM modes.";
            return true;
        }
        LOG(Error) << "Failed to write " << name() << " PWM modes." << quanser_msg(result);
        return false;
    };
    modes.on_write.connect(mode_write_impl);
    // Write frequencies
    auto freq_write_impl = [this](const ChanNum* chs, const double* vals, std::size_t n) {
        auto result = hil_set_pwm_frequency(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0) {
            LOG(Verbose) << "Wrote " << name() << " PWM frequencies.";
            return true;
        }
        LOG(Error) << "Failed to write " << name() << " PWM frequencies. " << quanser_msg(result);
        return false; 
    };
    frequencies.on_write.connect(freq_write_impl);
    // Write duty cycles
    auto duty_write_impl = [this](const ChanNum* chs, const double* vals, std::size_t n) {
        auto result = hil_set_pwm_duty_cycle(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0) {
            LOG(Verbose) << "Wrote " << name() << " PWM duty cycles.";
            return true;
        }
        LOG(Error) << "Failed to write " << name() << " PWM duty cycles. " << quanser_msg(result);
        return false; 
    };
    duty_cycles.on_write.connect(duty_write_impl);
    /// on channels gained
    auto on_gain = [this](const ChanNums& gained) {
        return modes.write(gained, std::vector<Mode>(gained.size(), Mode::DutyCycle)) && 
               frequencies.write(gained, std::vector<double>(gained.size(), 10000)) && 
               expire_values.write(gained, std::vector<double>(gained.size(), 0));
    };
    on_gain_channels.connect(on_gain);
}

} // namespace daq 
} // namespace mahi