#include <Mahi/Daq/ModuleInterface.hpp>  
#include <Mahi/Daq/Module.hpp>
#include <Mahi/Daq/DaqBase.hpp>

namespace mahi {
namespace daq {

ModuleInterfaceBase::ModuleInterfaceBase(Module& module) :
    m_module(module) 
{
    module.m_ifaces.push_back(this);    
}

ChanNum ModuleInterfaceBase::intern(ChanNum public_facing) {
    return m_module.transform_channel_number(public_facing);
}

Readable::Readable(Module& module) : read_with_all(false)
{
    module.m_daq.m_readables.push_back(this);
}

Writeable::Writeable(Module& module) : write_with_all(false)
{
    module.m_daq.m_writeables.push_back(this);
}


} // namespace daq
} // namespace mahi