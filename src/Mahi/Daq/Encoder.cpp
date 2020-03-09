#include <Mahi/Daq/Encoder.hpp>
#include <Mahi/Util/Logging/Log.hpp>

namespace mahi {
namespace daq {

//==============================================================================
// MODULE DEFINITIONS
//==============================================================================

Encoder::Encoder() :
    Module(),
    factors_(this, X4),
    units_per_count_(this, 1.0),
    positions_(this),
    values_per_sec_(this),
    velocities_(this),
    conversions_(this)
{
    compute_conversions();
}

Encoder::Encoder(const ChanNums& channel_numbers) :
    Module<int>(channel_numbers),
    factors_(this, X4),
    units_per_count_(this, 1.0),
    positions_(this),
    values_per_sec_(this),
    velocities_(this),
    conversions_(this)
{
    compute_conversions();
}

Encoder::~Encoder() {

}

bool Encoder::on_enable() {
    compute_conversions();
    return true;
}

bool Encoder::reset_counts(const std::vector<int>& counts) {
    if (valid_count(counts.size())) {
        auto ch_nums = channel_numbers();
        for (std::size_t i = 0; i < ch_nums.size(); ++i) {
            reset_count(ch_nums[i], counts[i]);
        }
    }
    return true;
}

bool Encoder::reset_count(ChanNum channel_number, int count) {
    if (valid_channel(channel_number)) {
        m_values[channel_number] = count;
        return true;
    }
    return false;
}


bool Encoder::set_quadrature_factors(const std::vector<QuadFactor>& factors) {
    factors_.set(factors);
    compute_conversions();
    return true;
}

bool Encoder::set_quadrature_factor(ChanNum channel_number, QuadFactor factor) {
    if (valid_channel(channel_number)) {
        factors_[channel_number] = factor;
        compute_conversions();
        return true;
    }
    return false;
}

bool Encoder::zero() {
    std::vector<int> zero(channel_count(), 0);
    return reset_counts(zero);
}

bool Encoder::zero_channel(ChanNum channel_number) {
    int zero = 0;
    return reset_count(channel_number, zero);
}

void Encoder::set_units_per_count(const std::vector<double>& units_per_count) {
    units_per_count_.set(units_per_count);
    compute_conversions();
}

void Encoder::set_units_per_count(ChanNum channel_number, double units_per_count) {
    if (valid_channel(channel_number)) {
        units_per_count_[channel_number] = units_per_count;
        compute_conversions();
    }
}

const std::vector<double>& Encoder::get_positions() const {
    // for (auto const& ch : channel_numbers())
    //     positions_[ch] = m_values[ch] * conversions_[ch];
    return positions_.get();
}

std::vector<double>& Encoder::get_positions() {
    for (auto const& ch : channel_numbers())
        positions_[ch] = m_values[ch] * conversions_[ch];
    return positions_.get();
}

double Encoder::get_position(ChanNum channel_number) {
    if (valid_channel(channel_number)) {
        return m_values[channel_number] * conversions_[channel_number];
    }
    else
        return double();
}

const std::vector<double>& Encoder::get_values_per_sec() const {
    if (!has_velocity()) {
        LOG(util::Warning) << "Encoder module " << get_name() << " has no velocity estimation";
    }
    return values_per_sec_.get();
}

std::vector<double>& Encoder::get_values_per_sec() {
    if (!has_velocity()) {
        LOG(util::Warning) << "Encoder module " << get_name() << " has no velocity estimation";
    }
    return values_per_sec_.get();
}

double Encoder::get_value_per_sec(ChanNum channel_number) {
    if (!has_velocity()) {
        LOG(util::Warning) << "Encoder module " << get_name() << " has no velocity estimation";
    }
    if (valid_channel(channel_number))
        return values_per_sec_[channel_number];
    else
        return double();
}

const std::vector<double>& Encoder::get_velocities() const {
    if (!has_velocity()) {
        LOG(util::Warning) << "Encoder module " << get_name() << " has no velocity estimation";
    }
    // for (auto const& ch : channel_numbers())
    //     velocities_[ch] = values_per_sec_[ch] * conversions_[ch];
    return velocities_.get();
}

std::vector<double>& Encoder::get_velocities() {
    if (!has_velocity()) {
        LOG(util::Warning) << "Encoder module " << get_name() << " has no velocity estimation";
    }
    for (auto const& ch : channel_numbers())
        velocities_[ch] = values_per_sec_[ch] * conversions_[ch];
    return velocities_.get();
}

double Encoder::get_velocity(ChanNum channel_number) {
    if (!has_velocity()) {
        LOG(util::Warning) << "Encoder module " << get_name() << " has no velocity estimation";
    }
    if (valid_channel(channel_number)) {
        return values_per_sec_[channel_number] * conversions_[channel_number];
    }
    else
        return double();
}

Encoder::Channel Encoder::channel(ChanNum channel_number) {
    if (valid_channel(channel_number))
        return Channel(this, channel_number);
    else
        return Channel();
}

bool Encoder::has_velocity() const {
    return false;
}

void Encoder::compute_conversions() {
    for (std::size_t i = 0; i < channel_count(); ++i) {
        ChanNum ch = channel_numbers()[i];
        conversions_[ch] = units_per_count_[ch] / static_cast<double>(factors_[ch]);
    }
}

Encoder::Channel::Channel() :
    ChannelBase()
{ }

Encoder::Channel::Channel(Encoder* module, ChanNum channel_number) :
    ChannelBase(module, channel_number)
{ }

double Encoder::Channel::get_position() {
    return static_cast<Encoder*>(m_module)->get_position(channel_number_);
}

bool Encoder::Channel::zero() {
    return static_cast<Encoder*>(m_module)->zero_channel(channel_number_);
}

bool Encoder::Channel::reset_count(int count) {
    return static_cast<Encoder*>(m_module)->reset_count(channel_number_, count);
}

bool Encoder::Channel::set_quadrature_factor(QuadFactor factor) {
    return static_cast<Encoder*>(m_module)->set_quadrature_factor(channel_number_, factor);
}

void Encoder::Channel::set_units_per_count(double units_per_count) {
    static_cast<Encoder*>(m_module)->set_units_per_count(channel_number_, units_per_count);
}

double Encoder::Channel::get_value_per_sec() {
    return static_cast<Encoder*>(m_module)->get_value_per_sec(channel_number_);
}

double Encoder::Channel::get_velocity() {
    return static_cast<Encoder*>(m_module)->get_velocity(channel_number_);
}

} // namespace daq
} // namespace mahi
