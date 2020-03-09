namespace mahi {
namespace daq {

    template <typename T>
    InputOutput<T>::InputOutput() :
        Output<T>(),
        directions_(this, Direction::In)
    {
        sort_input_output_channel_numbers();
    }

    template <typename T>
    InputOutput<T>::InputOutput(const ChanNums& channel_numbers) :
        Output<T>(channel_numbers),
        directions_(this, Direction::In)
    {
        sort_input_output_channel_numbers();
    }

    template <typename T>
    InputOutput<T>::~InputOutput() {}

    template <typename T>
    bool InputOutput<T>::set_directions(const std::vector<Direction>& directions) {
        directions_.set(directions);
        sort_input_output_channel_numbers();
        return true;
    }

    template <typename T>
    bool InputOutput<T>::set_direction(ChanNum channel_number, Direction direction) {
        if (this->valid_channel(channel_number)) {
            directions_[channel_number] = direction;
            sort_input_output_channel_numbers();
            return true;
        }
        return false;
    }

    template <typename T>
    bool InputOutput<T>::update_input() {
        auto chnums = input_channel_numbers();
        bool success = true;
        for (auto& c : chnums)
            success = this->update_channel(c) ? success : false;
        return success;
    }

    template <typename T>
    bool InputOutput<T>::update_output() {
        
        auto chnums = output_channel_numbers();
        bool success = true;
        for (auto& c : chnums)
            success = this->update_channel(c) ? success : false;
        return success;
    }

    template <typename T>
    std::size_t InputOutput<T>::input_channel_count() const {
        return input_channel_numbers_.size();
    }

    template <typename T>
    std::size_t InputOutput<T>::output_channel_count() const {
        return output_channel_numbers_.size();
    }

    template <typename T>
    const ChanNums& InputOutput<T>::input_channel_numbers() const {
        return input_channel_numbers_;
    }

    template <typename T>
    const ChanNums& InputOutput<T>::output_channel_numbers() const {
        return output_channel_numbers_;
    }

    template <typename T>
    typename InputOutput<T>::Channel InputOutput<T>::channel(ChanNum channel_number) {
        if (this->valid_channel(channel_number))
            return Channel(this, channel_number);
        else
            return Channel();
    }

    template <typename T>
    void InputOutput<T>::set_channel_numbers(const ChanNums& channel_numbers) {
        Output<T>::set_channel_numbers(channel_numbers);
        sort_input_output_channel_numbers();
    }

    template <typename T>
    void InputOutput<T>::add_channel_number(ChanNum channel_number) {
        Output<T>::add_channel_number(channel_number);
        sort_input_output_channel_numbers();
    }

    template <typename T>
    void InputOutput<T>::remove_channel_number(ChanNum channel_number) {
        Output<T>::remove_channel_number(channel_number);
        sort_input_output_channel_numbers();
    }

    template <typename T>
    void InputOutput<T>::sort_input_output_channel_numbers() const {
        input_channel_numbers_.clear();
        output_channel_numbers_.clear();
        for (std::size_t i = 0; i < this->channels_internal().size(); ++i) {
            if (directions_.get()[i] == In)
                input_channel_numbers_.push_back(this->channels_internal()[i]);
            else if (directions_.get()[i] == Out)
                output_channel_numbers_.push_back(this->channels_internal()[i]);
        }
    }

    template <typename T>
    InputOutput<T>::Channel::Channel() : ChannelBase<T>() {}

    template <typename T>
    InputOutput<T>::Channel::Channel(InputOutput* module, ChanNum channel_number)
        : ChannelBase<T>(module, channel_number) {}

    template <typename T>
    void InputOutput<T>::Channel::set_direction(Direction direction) {
        dynamic_cast<InputOutput<T>*>(this->m_module)->set_direction(this->channel_number_, direction);
    }

} // namespace daq
} // namespace mahi
