#include <mahi/daq/Registry.hpp>  
#include <mahi/daq/Module.hpp>

namespace mahi {
namespace daq {

    RegistryBase::RegistryBase(ModuleBase* module) :
        module_(module) 
    {
        module_->add_registry(this);
    }

    std::size_t RegistryBase::index(ChanNum channel_number) const {
        return module_->index(channel_number);
    }


} // namespace daq
} // namespace mahi