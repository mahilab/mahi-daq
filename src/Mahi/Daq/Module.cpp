#include <Mahi/Daq/Module.hpp>
#include <Mahi/Daq/ModuleInterface.hpp>
#include <Mahi/Daq/Daq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <algorithm>
#include <map>

using namespace mahi::util;

namespace mahi {
namespace daq {

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
} // namespace private

Module::Module(Daq& daq) : m_daq(daq) {
    m_daq.m_modules.push_back(this);
}

const std::string& Module::name() const {
    return m_name;
}

void Module::set_name(const std::string& name) {
    m_name = name;
}

Daq& Module::daq() const {
    return m_daq;
}

ChannelsModule::ChannelsModule(Daq& daq) : Module(daq) { }

void ChannelsModule::set_channel_numbers(const ChanNums& channel_numbers) {
    auto new_channels = channel_numbers;
    sort_and_reduce_channels(new_channels);
    if (new_channels != m_chs_public) {
        auto old_channels = m_chs_public;
        m_chs_public = new_channels;
        update_internal();
        update_map();
        for (auto& iface : m_ifaces)
            iface->on_channels_changed.emit(old_channels, new_channels);
        LOG(Verbose) << "Set Module " << name() << " channel numbers to [" << m_chs_public << "]";
    }
}

void ChannelsModule::update_map() {
    ChanMap old_map = m_ch_map;
    m_ch_map = make_channel_map(m_chs_public);
    for (std::size_t i = 0; i < m_ifaces.size(); i++)
        m_ifaces[i]->remap_channels(old_map, m_ch_map);   
}

void ChannelsModule::update_internal() {
    m_chs_internal.resize(m_chs_public.size());
    for (int i = 0; i < m_chs_internal.size(); ++i)
        m_chs_internal[i] = transform_channel_number(m_chs_public[i]);
}

const ChanNums& ChannelsModule::channels() const {
    return m_chs_public;
}

const ChanNums& ChannelsModule::channels_internal() const {
    return m_chs_internal;
}

bool ChannelsModule::valid_channel(ChanNum channel_number, bool quiet) const {
    if (m_ch_map.count(channel_number) > 0)
        return true;
    if (!quiet) {
        LOG(Error) << "Invalid channel number " << channel_number << " not declared in channel numbers on Module " << name();
    }
    return false;
}

bool ChannelsModule::valid_count(std::size_t size, bool quiet) const {
    if (m_chs_public.size() == size)
        return true;
    if (!quiet) {
        LOG(Error) << "The number of elements (" << size << ") does not equal to channel count ("  << channels().size() << ") of Module " << name();
    }
    return false;
}

std::size_t ChannelsModule::index(ChanNum channel_number) const {
    return m_ch_map.at(channel_number);
}

ChanNum ChannelsModule::transform_channel_number(ChanNum public_facing) const {
    return public_facing;
}


// CHANNEL SHARING

namespace {
    std::unordered_map<ChannelsModule*,std::vector<std::pair<ChannelsModule*,ChannelsModule::ShareList>>> g_share_list_map; 
}

void ChannelsModule::share(ChannelsModule* a, ChannelsModule* b, ShareList share_list) {
    // make sure entries exist
    if (g_share_list_map.count(a) == 0)
        g_share_list_map[a] = {};
    if (g_share_list_map.count(b) == 0)
        g_share_list_map[b] = {};
    // add share lists for a
    g_share_list_map[a].push_back({b,share_list});
    // reverse share list
    ChanNums temp;
    for (auto& p : share_list)
    {
        temp = p.first;
        p.first = p.second;
        p.second = temp;
    }
    g_share_list_map[b].push_back({a,share_list});

    for (auto& entry : g_share_list_map) {
        for (auto& other : entry.second) {
            std::cout << entry.first->name() << " <=> " << other.first->name() << std::endl;
            for (auto& p : other.second) {
                std::cout << "  " << p.first << " <=> " << p.second << std::endl;
            }
        }
    }
}


} // namespace daq
} // namespace mahi
