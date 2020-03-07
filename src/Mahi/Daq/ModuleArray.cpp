#include <Mahi/Daq/ModuleArray.hpp>  
#include <Mahi/Daq/Module.hpp>

namespace mahi {
namespace daq {

    ModuleArrayBase::ModuleArrayBase(ModuleBase* module) :
        m_module(module) 
    {
        m_module->add_buffer(this);
    }

    std::size_t ModuleArrayBase::index(ChanNum channel_number) const {
        return m_module->index(channel_number);
    }

    std::size_t ModuleArrayBase::size() const {
        return m_module->channel_count();
    }

} // namespace daq
} // namespace mahi