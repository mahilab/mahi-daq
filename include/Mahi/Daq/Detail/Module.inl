#ifdef _MSC_VER
#pragma warning( disable : 4589 )
#endif

namespace mahi {
namespace daq {

template <typename T>
Module<T>::Module() :
    ModuleBase(),
    m_values(this),
    min_values_(this),
    max_values_(this)
{}

template <typename T>
Module<T>::Module(const ChanNums& channel_numbers) :
    ModuleBase(channel_numbers),
    m_values(this),
    min_values_(this),
    max_values_(this)
{}

template <typename T>
Module<T>::~Module() { }

template <typename T>
const T& Module<T>::operator[](ChanNum channel_number) const {
    return m_values[channel_number];
}

template <typename T>
T& Module<T>::operator[](ChanNum channel_number) {
    return m_values[channel_number];
}

template <typename T>
bool Module<T>::set_ranges(const std::vector<T>& min_values, const std::vector<T>& max_values) {
    min_values_.set(min_values);
    max_values_.set(max_values);
    return true;
}

template <typename T>
bool Module<T>::set_range(ChanNum channel_number, T min_value, T max_value) {
    if (validate_channel_number(channel_number)) {
        min_values_[channel_number] = min_value;
        max_values_[channel_number] = max_value;
        return true;
    }
    return false;
}

template <typename T>
std::vector<T>& Module<T>::get() {
    return m_values.get();
}

template <typename T>
const T& Module<T>::get(ChanNum channel_number) const {
    return m_values[channel_number];
}

template <typename T>
T& Module<T>::get(ChanNum channel_number) {
    return m_values[channel_number];
}

template <typename T>
void Module<T>::set(const std::vector<T>& values) {
    m_values.set(values);
}

template <typename T>
void Module<T>::set(ChanNum channel_number, T value) {
    m_values[channel_number] = value;
}

} // namespace daq
} // namespace mahi
