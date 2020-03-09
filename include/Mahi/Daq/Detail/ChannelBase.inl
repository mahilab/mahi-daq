namespace mahi {
namespace daq {

template <typename T>
ChannelBase<T>::ChannelBase() : m_module(nullptr), channel_number_(0) {}

template <typename T>
ChannelBase<T>::ChannelBase(Module<T>* module, ChanNum channel_number)
    : m_module(module), channel_number_(channel_number) {}

template <typename T>
ChannelBase<T>::~ChannelBase() {}

template <typename T>
bool ChannelBase<T>::update() {
    return m_module->update_channel(channel_number_);
}

template <typename T>
const T& ChannelBase<T>::get() const {
    return m_module->get(channel_number_);
}

template <typename T>
T& ChannelBase<T>::get() {
    return m_module->get(channel_number_);
}

template <typename T>
void ChannelBase<T>::set(T value) {
    m_module->set(channel_number_, value);
}

template <typename T>
ChanNum ChannelBase<T>::channel_number() const {
    return channel_number_;
}

template <typename T>
bool ChannelBase<T>::is_valid() const {
    if (m_module == nullptr)
        return false;
    if (!m_module->valid_channel(channel_number_))
        return false;
    return true;
}

} // namespace daq
} // namespace mahi
