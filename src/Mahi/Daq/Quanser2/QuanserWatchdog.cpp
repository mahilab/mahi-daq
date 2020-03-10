#include <hil.h>
#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <Mahi/Daq/Quanser2/QuanserWatchdog.hpp>
#include "QuanserUtils.hpp"

#include <Mahi/Util/Logging/Log.hpp>
using namespace mahi::util;

namespace mahi {
namespace daq {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QuanserWatchdog::QuanserWatchdog(QuanserDaq& d, QuanserHandle& h, util::Time timeout) :
    Watchdog(d),
    m_h(h)
{ 
    set_name(d.name() + ".watchdog");
}

QuanserWatchdog::~QuanserWatchdog() {

}

bool QuanserWatchdog::start() {
    t_error result = hil_watchdog_start(m_h, m_timout.as_seconds());
    if (result == 0) {
        LOG(Verbose) << "Started watchdog on " << name();
        return true;
    }
    else {
        LOG(Error) << "Failed to start watchdog on " << name() << " " << get_quanser_error_message(result);

        return false;
    }
}

bool QuanserWatchdog::kick() {
    t_error result;
    result = hil_watchdog_reload(m_h);
    if (result == 1) {
        return true;
    }
    else if (result == 0) {
        LOG(Warning) << "Watchdog on " << name() << " expired";
        m_watching = false;
        return false;
    }
    else {
        LOG(Error) << "Failed to kick watchdog on " << name() << " "  << get_quanser_error_message(result);
        return false;
    }
}

bool QuanserWatchdog::stop() {
    t_error result;
    result = hil_watchdog_stop(m_h);
    if (result == 0) {
        LOG(Verbose) << "Stopped watchdog on " << name();
        m_watching = false;
        return true;
    }
    else {
        LOG(Error) << "Failed to stop watchdog on " << name() << " "  << get_quanser_error_message(result);
        return false;
    }
}

bool QuanserWatchdog::is_expired() {
    t_error result;
    result = hil_watchdog_is_expired(m_h);
    if (result == 1) {
        m_watching = false;
        return true;
    }
    else if (result == 0) {
        return false;
    }
    else {
        LOG(Error) << "Failed to check expiration of watchdog on " << name() << " " << get_quanser_error_message(result);
        return false;
    }
}

bool QuanserWatchdog::clear() {
    t_error result;
    result = hil_watchdog_clear(m_h);
    if (result == 0) {
        LOG(Verbose) << "Cleared watchdog on " << name();
        return true;
    }
    else {
        LOG(Error) << "Failed to clear watchdog on " << name() << " " << get_quanser_error_message(result);
        return false;
    }
}

bool QuanserWatchdog::on_daq_open() {
    // clear watchdog (precautionary, ok if fails)
    stop();
    // clear the watchdog (precautionary, ok if fails)
    clear();
    return true;
}

bool QuanserWatchdog::on_daq_close() {
    // stop watchdog (precautionary, ok if fails)
    stop();
    // clear the watchdog (precautionary, ok if fails)
    clear();
    // allow changes to take effect
    util::sleep(milliseconds(10));
    return true;
}

} // namespace daq
} // namespace mahi
