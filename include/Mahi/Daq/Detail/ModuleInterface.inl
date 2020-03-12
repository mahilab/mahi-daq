#ifdef _MSC_VER
#pragma warning( disable : 4505 )
#endif

#include <type_traits>
#include <ostream>
#include <Mahi/Daq/Module.hpp>

namespace mahi {
namespace daq {

    template <typename T>
    ModuleInterface<T>::ModuleInterface(ChannelsModule& module, T default_value) :
        ModuleInterfaceBase(module),
        m_default(default_value)
    { 
        m_buffer.resize(this->module().channels_internal().size());        
        std::fill(m_buffer.begin(), m_buffer.end(), m_default);
    }

    /// Overload stream operator for ModuleInterface
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const ModuleInterface<T>& iface) {
        if (iface.module().channels_internal().size() > 0) {
            os << "{ ";
            for (std::size_t i = 0; i < iface.module().channels_internal().size() - 1; i++) {
                ChanNum ch = iface.module().channels_internal()[i];
                os << "[" << ch << "]:" << iface.m_buffer[iface.index(ch)] << ", ";
            }
            ChanNum ch = iface.module().channels_internal()[iface.module().channels_internal().size() - 1];
            os << "[" << ch << "]:" << iface.m_buffer[iface.index(ch)] << " }";
        }
        else {
            os << "{ }";
        }
        return os;
    }

    template <>
    inline std::ostream& operator<< <char>(std::ostream& os, const ModuleInterface<char>& iface) {
        if (iface.module().channels_internal().size() > 0) {
            os << "{ ";
            for (std::size_t i = 0; i < iface.module().channels_internal().size() - 1; i++) {
                ChanNum ch = iface.module().channels_internal()[i];
                os << "[" << ch << "]:" << (int)iface.m_buffer[iface.index(ch)] << ", ";
            }
            ChanNum ch = iface.module().channels_internal()[iface.module().channels_internal().size() - 1];
            os << "[" << ch << "]:" << (int)iface.m_buffer[iface.index(ch)] << " }";
        }
        else {
            os << "{ }";
        }
        return os;        return os;
    }

    template <typename T>
    void ModuleInterface<T>::remap_channels(const ChanMap& old_map, const ChanMap& new_map)
    {
        std::vector<T> new_values(new_map.size(), m_default);
        for (auto it = old_map.begin(); it != old_map.end(); ++it) {
            if (new_map.count(it->first))
                new_values[new_map.at(it->first)] = m_buffer[old_map.at(it->first)];
        }
        m_buffer = new_values;
    }

} // namespace daq
} // namespace mahi