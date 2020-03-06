namespace mahi {
namespace daq {

template <typename T>
ChannelBase<T>::ChannelBase() : module_(nullptr), channel_number_(0) {}

template <typename T>
ChannelBase<T>::ChannelBase(Module<T>* module, ChanNum channel_number)
    : module_(module), channel_number_(channel_number) {}

template <typename T>
ChannelBase<T>::~ChannelBase() {}

template <typename T>
bool ChannelBase<T>::update() {
    return module_->update_channel(channel_number_);
}

template <typename T>
const T& ChannelBase<T>::get() const {
    return module_->get(channel_number_);
}

template <typename T>
T& ChannelBase<T>::get() {
    return module_->get(channel_number_);
}

template <typename T>
void ChannelBase<T>::set(T value) {
    module_->set(channel_number_, value);
}

template <typename T>
ChanNum ChannelBase<T>::channel_number() const {
    return channel_number_;
}

template <typename T>
bool ChannelBase<T>::is_valid() const {
    if (module_ == nullptr)
        return false;
    if (!module_->validate_channel_number(channel_number_))
        return false;
    return true;
}

} // namespace daq
} // namespace mahi
