namespace mahi {
namespace daq {

    template<typename T>
    Input<T>::Input() : 
        Module<T>()
    {}

    template<typename T>
    Input<T>::Input(const ChanNums& channel_numbers) : 
        Module<T>(channel_numbers) 
    { }

    template<typename T>
    Input<T>::~Input() {}

    template<typename T>
    typename Input<T>::Channel Input<T>::channel(ChanNum channel_number) {
        if (this->validate_channel_number(channel_number))
            return Channel(this, channel_number);
        else
            return Channel();
    }

    template <typename T>
    Input<T>::Channel::Channel() : ChannelBase<T>() { }

    template <typename T>
    Input<T>::Channel::Channel(Input* module, ChanNum channel_number)
        : ChannelBase<T>(module, channel_number) { }

} // namespace daq
} // namespace mahi
