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

    template<typename T>
    std::vector<typename Input<T>::Channel> Input<T>::channels(
        const ChanNums& channel_numbers) {
        std::vector<Channel> channels;
        for (std::size_t i = 0; i < channel_numbers.size(); ++i)
            channels.push_back(channel(channel_numbers[i]));
        return channels;
    }

    template <typename T>
    Input<T>::Channel::Channel() : ChannelBase<T>() { }

    template <typename T>
    Input<T>::Channel::Channel(Input* module, ChanNum channel_number)
        : ChannelBase<T>(module, channel_number) { }

} // namespace daq
} // namespace mahi
