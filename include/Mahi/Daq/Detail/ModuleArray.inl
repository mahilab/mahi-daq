#ifdef _MSC_VER
#pragma warning( disable : 4505 )
#endif

namespace mahi {
namespace daq {

    template <typename T>
    ModuleArray<T>::ModuleArray(ModuleBase* module, T default_value) :
        ModuleArrayBase(module),
        m_default_value(default_value)
    { 
        m_values.resize(this->m_module->channel_count());        
        std::fill(m_values.begin(), m_values.end(), m_default_value);
    }

    template <typename T>
    const T& ModuleArray<T>::operator[](ChanNum channel_number) const {
        return m_values[index(channel_number)];
    }

    template <typename T>
    void ModuleArray<T>::set_default(T default_value) {
        m_default_value = default_value;
    }

    /// Overload stream operator for ModuleArray
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const ModuleArray<T>& array) {
        if (array.size() > 0) {
            os << "{";
            for (std::size_t i = 0; i < array.size() - 1; i++) {
                ChanNum ch = array.m_module->channel_numbers()[i];
                os << "[" << ch << "]:" << array[ch] << ", ";
            }
            ChanNum ch = array.m_module->channel_numbers()[array.size() - 1];
            os << "[" << ch << "]:" << array[ch] << "}";
        }
        else {
            os << "{}";
        }
        return os;
    }

    template <typename T>
    void ModuleArray<T>::change_channel_numbers(const ChanMap& old_map, const ChanMap& new_map)
    {
        std::vector<T> new_values(new_map.size(), m_default_value);
        for (auto it = old_map.begin(); it != old_map.end(); ++it) {
            if (new_map.count(it->first))
                new_values[new_map.at(it->first)] = m_values[old_map.at(it->first)];
        }
        m_values = new_values;
    }

} // namespace daq
} // namespace mahi