#include <Mahi/Daq/DaqBase.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <Mahi/Daq/ModuleInterface.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

DaqBase::DaqBase()
{ }

DaqBase::~DaqBase() {
    // this once call disable/close, but calling virtual functions in ctor/dtor
    // is considered dangerous!
}

/// Reads all readable ModuleInterfaces owned
bool DaqBase::read_all() {
    bool success = true;
    for (auto& r : m_readables) {
        if (r->read_with_all)
            success = r->read() ? success : false;
    }
    return success;
}

/// Reads all writeable ModuleInterfaces owned
bool DaqBase::write_all() {
    bool success = true;
    for (auto& w : m_writeables) {
        if (w->write_with_all)
            success = w->write() ? success : false;
    }
    return success;
}

const std::vector<Module*>& DaqBase::modules() const {
    return m_modules;
}

// void DaqBase::add_readable(Readable* readable) {
//     if (std::find(m_readables.begin(), m_readables.end(), readable) == m_readables.end())
//         m_readables.push_back(readable);
// }

// void DaqBase::add_writeable(Writeable* writeable) {
//     if (std::find(m_writeables.begin(), m_writeables.end(), writeable) == m_writeables.end())
//         m_writeables.push_back(writeable);
// }

// void DaqBase::rem_readable(Readable* readable) {
//     m_readables.erase( std::remove( m_readables.begin(), m_readables.end(), readable ), m_readables.end() );
// }

// void DaqBase::rem_writeable(Writeable* writeable) {
//     m_writeables.erase( std::remove( m_writeables.begin(), m_writeables.end(), writeable ), m_writeables.end() );
// }

} // namespace daq
} // namespace mahi
