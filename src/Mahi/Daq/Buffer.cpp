#include <mahi/daq/Buffer.hpp>  
#include <mahi/daq/Module.hpp>

namespace mahi {
namespace daq {

    BufferBase::BufferBase(ModuleBase* module) :
        module_(module) 
    {
        module_->add_buffer(this);
    }

    std::size_t BufferBase::index(ChanNum channel_number) const {
        return module_->index(channel_number);
    }


} // namespace daq
} // namespace mahi