#ifdef _MSC_VER
#pragma warning( disable : 4505 )
#endif

namespace mahi {
namespace daq {

    template <typename T>
    Buffer<T>::Buffer(ModuleBase* module, T default_value) :
        BufferBase(module),
        default_value_(default_value)
    { 
        raw_values_.resize(this->module_->channel_count());        
        std::fill(raw_values_.begin(), raw_values_.end(), default_value_);
    }

    template <typename T>
    const T& Buffer<T>::operator[](ChanNum channel_number) const {
        return raw_values_[index(channel_number)];
    }

    template <typename T>
    T& Buffer<T>::operator[](ChanNum channel_number) {
        return raw_values_[index(channel_number)];
    }

    template <typename T>
    std::size_t Buffer<T>::size() const {
        return raw_values_.size();
    }

    template <typename T>
    std::vector<T>& Buffer<T>::get_raw() {
        return raw_values_;
    }

    template <typename T>
    const std::vector<T> &Buffer<T>::get_raw() const {
        return raw_values_;
    }

    template <typename T>
    void Buffer<T>::set_raw(const std::vector<T>& values) {
        if (raw_values_.size() == values.size())
            raw_values_ = values;
    }

    template <typename T>
    void Buffer<T>::set_default_value(T default_value) {
        default_value_ = default_value;
    }

    template <typename T>
    void Buffer<T>::change_channel_numbers(const ChanMap& old_map,
                                const ChanMap& new_map)
    {
        std::vector<T> new_values(new_map.size(), default_value_);
        for (auto it = old_map.begin(); it != old_map.end(); ++it) {
            if (new_map.count(it->first))
                new_values[new_map.at(it->first)] = raw_values_[old_map.at(it->first)];
        }
        raw_values_ = new_values;
    }

    /// Overload stream operator for Buffer
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Buffer<T>& buffer) {
        if (buffer.size() > 0) {
            os << "{";
            for (std::size_t i = 0; i < buffer.size() - 1; i++) {
                ChanNum ch = buffer.module_->channel_numbers()[i];
                os << "[" << ch << "]:" << buffer[ch] << ", ";
            }
            ChanNum ch = buffer.module_->channel_numbers()[buffer.size() - 1];
            os << "[" << ch << "]:" << buffer[ch] << "}";
        }
        else {
            os << "{}";
        }
        return os;
    }

} // namespace daq
} // namespace mahi
