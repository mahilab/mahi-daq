#ifdef _MSC_VER
#pragma warning( disable : 4505 )
#endif

#include <type_traits>
#include <ostream>
#include <Mahi/Daq/Module.hpp>

namespace mahi {
namespace daq {

template <typename T>
Buffer<T>::Buffer(ChanneledModule& module, T default_value) :
    BufferBase(module),
    m_default(default_value)
{ 
    m_buffer.resize(this->module().channels_internal().size());        
    std::fill(m_buffer.begin(), m_buffer.end(), m_default);
}

/// Overload stream operator for Buffer
template <typename T>
std::ostream& operator<<(std::ostream& os, const Buffer<T>& iface) {
    if (iface.module().channels().size() > 0) {
        os << "{ ";
        for (std::size_t i = 0; i < iface.module().channels().size() - 1; i++) {
            ChanNum ch = iface.module().channels()[i];
            os << "[" << ch << "]:" << iface.m_buffer[iface.index(ch)] << ", ";
        }
        ChanNum ch = iface.module().channels()[iface.module().channels().size() - 1];
        os << "[" << ch << "]:" << iface.m_buffer[iface.index(ch)] << " }";
    }
    else {
        os << "{ }";
    }
    return os;
}

template <>
inline std::ostream& operator<< <char>(std::ostream& os, const Buffer<char>& iface) {
    if (iface.module().channels().size() > 0) {
        os << "{ ";
        for (std::size_t i = 0; i < iface.module().channels().size() - 1; i++) {
            ChanNum ch = iface.module().channels()[i];
            os << "[" << ch << "]:" << (int)iface.m_buffer[iface.index(ch)] << ", ";
        }
        ChanNum ch = iface.module().channels()[iface.module().channels().size() - 1];
        os << "[" << ch << "]:" << (int)iface.m_buffer[iface.index(ch)] << " }";
    }
    else {
        os << "{ }";
    }
    return os;        return os;
}

template <typename T>
void Buffer<T>::remap(const ChanMap& old_map, const ChanMap& new_map)
{
    std::vector<T> new_values(new_map.size(), m_default);
    for (auto it = old_map.begin(); it != old_map.end(); ++it) {
        if (new_map.count(it->first))
            new_values[new_map.at(it->first)] = m_buffer[old_map.at(it->first)];
    }
    m_buffer = new_values;
}

//==============================================================================
// MIXINS
//==============================================================================




} // namespace daq
} // namespace mahi