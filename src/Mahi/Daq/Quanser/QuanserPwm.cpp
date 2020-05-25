#include <hil.h>

#include <Mahi/Daq/Quanser/QuanserDaq.hpp>
#include <Mahi/Daq/Quanser/QuanserPwm.hpp>
#include <Mahi/Util/Logging/Log.hpp>

#include "QuanserUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

QuanserPwm::QuanserPwm(QuanserDaq& d, QuanserHandle& h, const ChanNums& allowed,
                       std::function<bool(const ChanNums&)> on_gain_custom,
                       std::function<bool(const ChanNums&)> on_free_custom) :
    PwmModuleBasic(d, allowed),
    m_on_gain_custom(on_gain_custom),
    m_on_free_custom(on_free_custom),
    modes(*this, Mode::DutyCycle),
    expire_values(*this, 0),
    frequencies(*this, 10000),
    duty_cycles(*this, 0.5),
    m_h(h) {
    set_name(d.name() + ".PWM");
    // Write Channels
    auto write_impl = [this](const ChanNum* chs, const double* vals, std::size_t n) {
        t_error result = hil_write_pwm(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result != 0) {
            LOG(Error) << "Failed to write " << this->name() << " PWM outputs. "
                       << quanser_msg(result);
            return false;
        }
        return true;
    };
    connect_write(*this, write_impl);
    // Write Expire States
    auto expire_write_impl = [this](const ChanNum* chs, const double* vals, std::size_t n) {
        t_error result =
            hil_watchdog_set_pwm_expiration_state(m_h, chs, static_cast<t_uint32>(n), vals);
        if (result == 0) {
            LOG(Verbose) << "Set " << name() << " expire PWM expiration states.";
            return true;
        }
        LOG(Error) << "Failed to set " << name() << " PWM expiration states. "
                   << quanser_msg(result);
        return false;
    };
    connect_write(expire_values, expire_write_impl);
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
    connect_write(modes, mode_write_impl);
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
    connect_write(frequencies, freq_write_impl);
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
    connect_write(duty_cycles, duty_write_impl);
}

bool QuanserPwm::init_channels(const ChanNums& chs) {
    bool success = true;
    if (m_on_gain_custom) {
        success = m_on_gain_custom(chs);
    }
    return success && modes.write(chs, std::vector<Mode>(chs.size(), Mode::DutyCycle)) &&
           frequencies.write(chs, std::vector<double>(chs.size(), 10000)) &&
           expire_values.write(chs, std::vector<double>(chs.size(), 0));
}

bool QuanserPwm::on_daq_open() {
    return init_channels(channels());
}

bool QuanserPwm::on_gain_channels(const ChanNums& chs) {
    if (!daq().is_open())
        return true;
    return init_channels(chs);
}

bool QuanserPwm::on_free_channels(const ChanNums& chs) {
    if (m_on_free_custom)
        return m_on_free_custom(chs);
    else
        return true;
}

}  // namespace daq
}  // namespace mahi