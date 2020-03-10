#include <Mahi/Daq/ModuleInterface.hpp>  
#include <Mahi/Daq/Module.hpp>
#include <Mahi/Daq/Daq.hpp>

namespace mahi {
namespace daq {

ModuleInterfaceBase::ModuleInterfaceBase(ChannelsModule& module) :
    m_module(module) 
{
    module.m_ifaces.push_back(this);    
}

ChanNum ModuleInterfaceBase::intern(ChanNum public_facing) {
    return m_module.transform_channel_number(public_facing);
}

Readable::Readable(ChannelsModule& module) : read_with_all(false)
{
    module.daq().m_readables.push_back(this);
}

Writeable::Writeable(ChannelsModule& module) : write_with_all(false)
{
    module.daq().m_writeables.push_back(this);
}


} // namespace daq
} // namespace mahi