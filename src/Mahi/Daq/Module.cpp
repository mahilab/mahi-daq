#include <Mahi/Daq/Module.hpp>
#include <Mahi/Daq/ModuleInterface.hpp>
#include <Mahi/Daq/Daq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <algorithm>
#include <map>
#include <Mahi/Util/Print.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

namespace  {

inline void sort_and_reduce(ChanNums& chs) {
    std::sort(chs.begin(), chs.end());
    chs.erase(std::unique(chs.begin(), chs.end()), chs.end());
}

inline bool contains(const ChanNums& chs, ChanNum ch) {
    return std::count(chs.begin(), chs.end(), ch) > 0;
}

inline ChanMap make_channel_map(const ChanNums& channel_numbers) {
    ChanMap channel_map;
    for (std::size_t i = 0; i < channel_numbers.size(); ++i)
        channel_map[channel_numbers[i]] = i;
    return channel_map;
}

std::unordered_map<ChannelsModule*,std::vector<std::pair<ChannelsModule*,ChannelsModule::ShareList>>> g_share_list_map; 

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

ChannelsModule::ChannelsModule(Daq& daq, const ChanNums& allowed) : 
    Module(daq),
    m_chs_allowed(allowed)
{ }

bool ChannelsModule::set_channels(const ChanNums& chs) {
    // sort and reduce the requested channels
    auto requested = chs;
    sort_and_reduce(requested);
    // return early if these are already the channels
    if (requested == m_chs_public) 
        return true;
    // check that the requested channels are allowed
    if (m_chs_allowed.size() > 0) {
            bool proceed = true;
        for (auto& req : chs) {
            if (!contains(m_chs_allowed, req)) {
                LOG(Error) << "Channel " << req << " now allowed on Module " << name() << ". Allowed channels are [" << m_chs_allowed << "].";
                proceed = false;
            }
        }
        if (!proceed)
            return false;
    }
    // save the previous channels
    auto previous = m_chs_public;
    // determine channels gained
    ChanNums gained;
    for (auto& req : requested) {
        if (!contains(previous, req))
            gained.push_back(req);
    }
    // determine channels freed
    ChanNums freed;
    for (auto& prev : previous) {
        if (!contains(requested, prev))
            freed.push_back(prev);
    }

    // set the new channels
    m_chs_public = requested;
    // update internal representation
    m_chs_internal.resize(m_chs_public.size());
    for (int i = 0; i < m_chs_internal.size(); ++i)
        m_chs_internal[i] = transform_channel_number(m_chs_public[i]);
    // remap channels
    ChanMap old_map = m_ch_map;
    m_ch_map = make_channel_map(m_chs_public);
    for (std::size_t i = 0; i < m_ifaces.size(); i++)
        m_ifaces[i]->remap_channels(old_map, m_ch_map); 
    // relinquish shared pins
    if (shares_pins()) {
        for (auto relation : g_share_list_map[this]) {
            ChannelsModule* other = relation.first;
            ChanNums must_remove;
            for (auto& mapping : relation.second) {
                for (auto& lch : mapping.first) {
                    if (std::count(m_chs_public.begin(), m_chs_public.end(), lch)) {
                        for (auto& rch : mapping.second)
                            must_remove.push_back(rch);
                        break;
                    }
                }
            }
            ChanNums remaining;
            for (auto& existing : other->channels()) {
                if (!std::count(must_remove.begin(), must_remove.end(), existing))
                    remaining.push_back(existing);
            }
            if (must_remove.size() > 0)
                other->set_channels(remaining);
        }
    }
    // invoke callbacks

    // LOG(Verbose) << "Set Module " << name() << " channel numbers to [" << m_chs_public << "].";
    if (gained.size() > 0) {
        on_gain_channels.emit(gained);
        LOG(Verbose) << "Module " << name() << " gained channel numbers [" << gained << "].";
    }
    if (freed.size() > 0) {
        on_free_channels.emit(freed);
        LOG(Verbose) << "Module " << name() << " freed channel numbers [" << freed << "].";
    }
    return true;
}

const ChanNums& ChannelsModule::channels() const {
    return m_chs_public;
}

const ChanNums& ChannelsModule::channels_internal() const {
    return m_chs_internal;
}

ChanNum ChannelsModule::transform_channel_number(ChanNum public_facing) const {
    return public_facing;
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
}

bool ChannelsModule::shares_pins() const {
    for (auto& entry : g_share_list_map) {
        if (entry.first == this)
            return true;
    }
    return false;
}

// void ChannelsModule::print_shared_pins() {
//     for (auto& entry : g_share_list_map) {
//         for (auto& other : entry.second) {
//             std::cout << entry.first->name() << " <=> " << other.first->name() << std::endl;
//             for (auto& p : other.second) {
//                 std::cout << "  " << p.first << " <=> " << p.second << std::endl;
//             }
//         }
//     }   
// }

} // namespace daq
} // namespace mahi
