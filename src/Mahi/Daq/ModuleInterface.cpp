#include <Mahi/Daq/ModuleInterface.hpp>  
#include <Mahi/Daq/Module.hpp>
#include <Mahi/Daq/Daq.hpp>
#include <Mahi/Util/Logging/Log.hpp>

using namespace mahi::util;

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

const ChannelsModule& ModuleInterfaceBase::module() const {
    return m_module;
}

bool ModuleInterfaceBase::valid_channel(ChanNum channel_number, bool quiet) const {
    if (m_module.m_ch_map.count(channel_number) > 0)
        return true;
    if (!quiet) {
        LOG(Error) << "Invalid channel number " << channel_number << " not declared in channel numbers on Module " << m_module.name() << ".";
    }
    return false;
}

bool ModuleInterfaceBase::valid_count(std::size_t size, bool quiet) const {
    if (m_module.m_chs_public.size() == size)
        return true;
    if (!quiet) {
        LOG(Error) << "The number of elements (" << size << ") does not equal to channel count ("  << m_module.channels().size() << ") of Module " << m_module.name() << ".";
    }
    return false;
}

std::size_t ModuleInterfaceBase::index(ChanNum channel_number) const {
    return m_module.m_ch_map.at(channel_number);
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