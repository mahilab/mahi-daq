#include <Mahi/Daq/DaqBase.hpp>

#include <Mahi/Util/Logging/Log.hpp>
using namespace mahi::util;

namespace mahi {
namespace daq {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

DaqBase::DaqBase(const std::string& name) :
    Device(name)
{

}

DaqBase::~DaqBase() {
    // this once call disable/close, but calling virtual functions in ctor/dtor
    // is considered dangerous!
}

bool DaqBase::update_input() {
    // TODO?
    return true;
}

bool DaqBase::update_output() {
    // TODO?
    return true;
}

} // namespace daq
} // namespace mahi
