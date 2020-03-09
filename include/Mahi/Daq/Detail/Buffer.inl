#ifdef _MSC_VER
#pragma warning( disable : 4505 )
#endif

namespace mahi {
namespace daq {

    template <typename T>
    Buffer<T>::Buffer(ModuleInterface* module, T default_value) :
        ModuleInterface<T>(module, default_value)
    { 
    }

    template <typename T>
    T& Buffer<T>::operator[](ChanNum channel_number) {
        return m_values[index(channel_number)];
    }

    template <typename T>
    std::vector<T>& Buffer<T>::get() {
        return m_values;
    }

    template <typename T>
    const std::vector<T> &Buffer<T>::get() const {
        return m_values;
    }

    template <typename T>
    void Buffer<T>::set(const std::vector<T>& values) {
        if (m_values.size() == values.size())
            m_values = values;
    }




} // namespace daq
} // namespace mahi
