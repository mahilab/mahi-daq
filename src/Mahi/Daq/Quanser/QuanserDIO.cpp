#include <hil.h>
#include <Mahi/Daq/Quanser/QuanserDaq.hpp>
#include <Mahi/Daq/Quanser/QuanserDIO.hpp>

#include <Mahi/Util/Logging/Log.hpp>
using namespace mahi::util;

namespace mahi {
namespace daq {

    //==============================================================================
    // CLASS DEFINITIONS
    //==============================================================================

    QuanserDIO::QuanserDIO(QuanserDaq& daq, const ChanNums& channel_numbers) :
        DigitalInputOutput(channel_numbers),
        daq_(daq)
    {
        set_name(daq_.get_name() + "_DIO");
        sort_input_output_channel_numbers();
    }

    bool QuanserDIO::on_enable() {
        set(enable_values_.get());
        if (update()) {
            LOG(Verbose) << "Set " << get_name() << " enable values to " << enable_values_;
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " enable values to " << enable_values_;
            return false;
        }
    }

    bool QuanserDIO::on_disable() {
        set(disable_values_.get());
        if (update()) {
            LOG(Verbose) << "Set " << get_name() << " disable values to " << disable_values_;
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " disable values to " << disable_values_;
            return false;
        }
    }

    bool QuanserDIO::update() {
        for (std::size_t i = 0; i < output_channel_count(); ++i)
            quanser_output_buffer_[i] = static_cast<char>(m_values[output_channel_numbers_[i]]);
        t_error result = hil_read_digital_write_digital(daq_.handle_,
            &input_channel_numbers_[0], static_cast<ChanNum>(input_channel_count()),
            &output_channel_numbers_[0], static_cast<ChanNum>(output_channel_count()),
            &quanser_input_buffer_[0], &quanser_output_buffer_[0]);
        for (std::size_t i = 0; i < input_channel_count(); ++i)
            m_values[input_channel_numbers_[i]] = static_cast<Logic>(quanser_input_buffer_[i]);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update " << get_name() << " " << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDIO::update_input() {
        t_error result = hil_read_digital(daq_.handle_, &input_channel_numbers_[0],  static_cast<ChanNum>(input_channel_count()), &quanser_input_buffer_[0]);
        for (std::size_t i = 0; i < input_channel_count(); ++i)
            m_values[input_channel_numbers_[i]] = static_cast<Logic>(quanser_input_buffer_[i]);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update inputs on " << get_name() << " " << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDIO::update_output() {
        for (std::size_t i = 0; i < output_channel_count(); ++i)
            quanser_output_buffer_[i] = static_cast<char>(m_values[output_channel_numbers_[i]]);
        t_error result = hil_write_digital(daq_.handle_, &output_channel_numbers_[0], static_cast<ChanNum>(output_channel_count()), &quanser_output_buffer_[0]);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update outputs on " << get_name() << " " << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDIO::update_channel(ChanNum channel_number) {
        char buffer;
        t_error result;
        if (directions_[channel_number] == In) {
            result = hil_read_digital(daq_.handle_, &channel_number, 1, &buffer);
            m_values[channel_number] = static_cast<Logic>(buffer);
        }
        else {
            buffer = static_cast<char>(m_values[channel_number]);
            result = hil_write_digital(daq_.handle_, &channel_number, 1, &buffer);
        }
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " "  << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDIO::set_directions(const std::vector<Direction>& directions) {
        if (!InputOutput::set_directions(directions))
            return false;
        t_error result;
        result = hil_set_digital_directions(daq_.handle_,
            &input_channel_numbers_[0], static_cast<ChanNum>(input_channel_count()),
            &output_channel_numbers_[0], static_cast<ChanNum>(output_channel_count()));
        if (result == 0) {
            LOG(Verbose) << "Set " << get_name() << " directions";
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " directions " << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDIO::set_direction(ChanNum channel_number, Direction direction) {
        if (!InputOutput::set_direction(channel_number, direction))
            return false;
        return set_directions(directions_.get());
    }

    bool QuanserDIO::set_expire_values(const std::vector<Logic>& expire_values) {
        if (!InputOutput::set_expire_values(expire_values))
            return false;
        // convert Logic to Quanser t_encoder_quadratue_mode
        std::vector<t_digital_state> converted_expire_values;
        for (auto it = expire_values.begin(); it != expire_values.end(); ++it) {
            if (*it == High)
                converted_expire_values.push_back(DIGITAL_STATE_HIGH);
            else
                converted_expire_values.push_back(DIGITAL_STATE_LOW);
        }
        t_error result;
        result = hil_watchdog_set_digital_expiration_state(daq_.handle_, &channel_numbers()[0], static_cast<ChanNum>(channel_count()), &converted_expire_values[0]);
        if (result == 0) {
            LOG(Verbose) << "Set " << get_name() << " expire values to " << expire_values_;
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " expire values " << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDIO::set_expire_value(ChanNum channel_number, Logic expire_value) {
        if (!InputOutput::set_expire_value(channel_number, expire_value))
            return false;
        // convert Logic to Quanser t_encoder_quadratue_mode
        t_digital_state converted_expire_value;
        if (expire_value == High)
            converted_expire_value = DIGITAL_STATE_HIGH;
        else
            converted_expire_value = DIGITAL_STATE_LOW;
        t_error result;
        result = hil_watchdog_set_digital_expiration_state(daq_.handle_, &channel_number, 1, &converted_expire_value);
        if (result == 0) {
            LOG(Verbose) << "Set " << get_name() << " channel number " << channel_number << " expire value to " << expire_value;
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " channel number " << channel_number << " expire value " << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    void QuanserDIO::sort_input_output_channel_numbers() const {
        DigitalInputOutput::sort_input_output_channel_numbers();
        quanser_input_buffer_.resize(input_channel_count());
        quanser_output_buffer_.resize(output_channel_count());
    }

    void QuanserDIO::buffer_to_input() {
        for (std::size_t i = 0; i < input_channel_count(); ++i)
           set(input_channel_numbers()[i], static_cast<Logic>(quanser_input_buffer_[i]));
    }

    void QuanserDIO::output_to_buffer() {
        for (std::size_t i = 0; i < output_channel_count(); ++i) 
            quanser_output_buffer_[i] = static_cast<char>(get(output_channel_numbers()[i]));
    }

} // namespace daq
} // namespace mahi
