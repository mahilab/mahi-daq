#include <Mahi/Daq/Module.hpp>
#include <Mahi/Daq/ModuleInterface.hpp>
#include <Mahi/Daq/DaqBase.hpp>
#include <algorithm>

#include <Mahi/Util/Logging/Log.hpp>
using namespace mahi::util;

namespace mahi {
namespace daq {

//==============================================================================
// HELPER FUNCTIONS
//==============================================================================

namespace  {

void sort_and_reduce_channels(ChanNums& channels) {
    std::sort(channels.begin(), channels.end());
    channels.erase(std::unique(channels.begin(), channels.end()), channels.end());
}

ChanMap make_channel_map(const ChanNums& channel_numbers) {
    ChanMap channel_map;
    for (std::size_t i = 0; i < channel_numbers.size(); ++i)
        channel_map[channel_numbers[i]] = i;
    return channel_map;
}

}

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Module::Module(DaqBase& daq) : m_daq(daq) { 
    m_daq.m_modules.push_back(this);
}

void Module::set_channel_numbers(const ChanNums& channel_numbers) {
    auto new_channels = channel_numbers;
    sort_and_reduce_channels(new_channels);
    if (new_channels != m_chs_public) {
        auto old_channels = m_chs_public;
        m_chs_public = new_channels;
        update_internal();
        update_map();
        for (auto& iface : m_ifaces)
            iface->on_channels_changed.emit(old_channels, new_channels);
        LOG(Verbose) << "Set Module " << get_name() << " channel numbers";// to " << m_chs;
    }
}

void Module::add_channel_number(ChanNum channel_number) {
    if (!m_ch_map.count(channel_number)) {
        auto old_channels = m_chs_public;
        m_chs_public.push_back(channel_number);
        sort_and_reduce_channels(m_chs_public);
        update_internal();
        update_map(); 
        for (auto& iface : m_ifaces)
            iface->on_channels_changed.emit(old_channels, m_chs_public);
        LOG(Verbose) << "Added channel number " << channel_number << " to Module " << get_name();      
    } 
}

void Module::remove_channel_number(ChanNum channel_number) {
    if (m_ch_map.count(channel_number)) {
        auto old_channels = m_chs_public;
        m_chs_public.erase(std::remove(m_chs_public.begin(), m_chs_public.end(), channel_number), m_chs_public.end());
        update_internal();
        update_map();
        for (auto& iface : m_ifaces)
            iface->on_channels_changed.emit(old_channels, m_chs_public);
        LOG(Verbose) << "Removed channel number " << channel_number << " from Module " << get_name();      
    }
}

void Module::update_map() {
    ChanMap old_map = m_ch_map;
    m_ch_map = make_channel_map(m_chs_public);
    for (std::size_t i = 0; i < m_ifaces.size(); i++)
        m_ifaces[i]->remap_channels(old_map, m_ch_map);   
}

void Module::update_internal() {
    m_chs_internal.resize(m_chs_public.size());
    for (int i = 0; i < m_chs_internal.size(); ++i)
        m_chs_internal[i] = transform_channel_number(m_chs_public[i]);
}

const ChanNums& Module::channels() const {
    return m_chs_public;
}

const ChanNums& Module::channels_internal() const {
    return m_chs_internal;
}

bool Module::valid_channel(ChanNum channel_number, bool quiet) const {
    if (m_ch_map.count(channel_number) > 0)
        return true;
    if (!quiet) {
        LOG(Error) << "Invalid channel number " << channel_number << " not declared in channel numbers on Module " << get_name();
    }
    return false;
}

bool Module::valid_count(std::size_t size, bool quiet) const {
    if (m_chs_public.size() == size)
        return true;
    if (!quiet) {
        LOG(Error) << "Invalid number of elements (" << size << ") not equal to channel count of " << channels_internal().size() << " on Module " << get_name();
    }
    return false;
}

std::size_t Module::index(ChanNum channel_number) const {
    return m_ch_map.at(channel_number);
}

ChanNum Module::transform_channel_number(ChanNum public_facing) const {
    return public_facing;
}


} // namespace daq
} // namespace mahi
