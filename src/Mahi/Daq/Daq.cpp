#include <Mahi/Daq/Daq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <Mahi/Daq/ModuleInterface.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

Daq::Daq()
{ }

Daq::~Daq() {
    // this once call disable/close, but calling virtual functions in ctor/dtor
    // is considered dangerous!
}

/// Reads all readable ModuleInterfaces owned
bool Daq::read_all() {
    bool success = true;
    for (auto& r : m_readables) {
        if (r->read_with_all)
            success = r->read() ? success : false;
    }
    return success;
}

/// Reads all writeable ModuleInterfaces owned
bool Daq::write_all() {
    bool all_success = true;
    for (auto& w : m_writeables) {
        if (w->write_with_all)
            all_success = w->write() ? all_success : false;
    }
    return all_success;
}

bool Daq::on_open() {
    if (on_daq_open()) {
        bool all_success = true;
        for (auto& m : m_modules) 
            all_success = m->on_daq_open() ? all_success : false;
        return all_success;        
    }
    return false;
}

bool Daq::on_close() {
    bool all_success = true;
    for (auto& m : m_modules) 
        all_success = m->on_daq_close() ? all_success : false;
    return on_daq_close() && all_success;        
}

bool Daq::on_enable() {
    if (!is_open()) {
        LOG(Error) << "Cannot enable " << name() << " because it is not open";
        return false;
    }
    if (on_daq_enable()) {
        bool all_success = true;
        for (auto& m : m_modules) 
            all_success = m->on_daq_enable() ? all_success : false;
        return all_success;        
    }
    return false;
}

bool Daq::on_disable() {
    if (!is_open()) {
        LOG(Error) << "Cannot disable " << name() << " because it is not open";
        return false;
    }
    bool all_success = true;
    for (auto& m : m_modules) 
        all_success = m->on_daq_disable() ? all_success : false;
    return on_daq_disable() && all_success;
}

const std::vector<Module*>& Daq::modules() const {
    return m_modules;
}

} // namespace daq
} // namespace mahi
