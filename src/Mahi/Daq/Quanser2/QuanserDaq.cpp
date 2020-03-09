#include <Mahi/Daq/Quanser2/QuanserDaq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <Mahi/Util/Print.hpp>
#include <hil.h>
#include <unordered_map>
#include <utility>

#include <Mahi/Daq/Quanser2/QuanserAO.hpp>
#include <Mahi/Daq/Quanser2/QuanserAI.hpp>
#include <Mahi/Daq/Quanser2/QuanserEncoder.hpp>
#include <Mahi/Daq/Quanser2/QuanserOther.hpp>

#include "QuanserUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

namespace {
std::unordered_map<const char*, int> g_id_map;

int inc_id(const char* card_type) {
    if (g_id_map.count(card_type))
        g_id_map[card_type]++;
    else
        g_id_map.emplace(card_type, 0);
    return g_id_map[card_type];
}

void dec_id(const char* card_type) {
    if (g_id_map.count(card_type))
        g_id_map[card_type]--;
}


}

QuanserDaq::QuanserDaq(const char* card_type) :
    m_card_type(card_type),
    m_id(inc_id(card_type)),
    m_handle(nullptr)
{
    set_name(std::string(m_card_type) + "_" + std::to_string(m_id));
}

QuanserDaq::~QuanserDaq() {
    dec_id(m_card_type);
}

bool QuanserDaq::read_all() {
    // m_other_read->m_values;
    print("Read");
    return true;
}

bool QuanserDaq::write_all() {
    print("Write");
    return true;
}

// bool QuanserDaq::read() {
//     return true;
// }

// bool QuanserDaq::write() {
//     if (!is_enabled()) {
//         LOG(Error) << "Unable to write " << get_name() << " because it is disabled";
//         return false;
//     }
//     // convert digitals
//     // for (std::size_t i = 0; i < DO.channels_internal().size(); ++i)
//     //     DO.get_quanser_values()[i] = static_cast<char>(DO.get()[i]);
//     t_error result;
//     // result = hil_write(m_handle,
//     //     AO.channels_internal().size() > 0 ? &(AO.channels_internal())[0] : nullptr, static_cast<ChanNum>(AO.channels_internal().size()),
//     //     nullptr, 0,
//     //     DO.channels_internal().size() > 0 ? &(DO.channels_internal())[0] : nullptr, static_cast<ChanNum>(DO.channels_internal().size()),
//     //     nullptr, 0,
//     //     AO.channels_internal().size() > 0 ? &(AO.get())[0] : nullptr,
//     //     nullptr,
//     //     DO.channels_internal().size() > 0 ? &(DO.get_quanser_values())[0] : nullptr,
//     //     nullptr);
//     result = hil_write(m_handle,
//         (m_AO && m_AO->channels_internal().size() > 0) ? &m_AO->channels_internal()[0] : nullptr, static_cast<t_uint32>(m_AO->channels_internal().size()),
//         nullptr, 0,
//         nullptr, 0,
//         nullptr, 0,
//         (m_AO && m_AO->channels_internal().size() > 0) ? &m_AO->m_values[0] : nullptr,
//         nullptr,
//         nullptr,
//         nullptr
//     );

//     if (result == 0)
//         return true;
//     else {
//         LOG(Error) << "Failed to update " << get_name() << " output " << get_quanser_error_message(result);
//         return false;
//     }
// }

bool QuanserDaq::set_options(const QuanserOptions& options) {
    m_options = options;
    char options_str[4096];
    std::strcpy(options_str, m_options.get_string().c_str());
    if (valid()) {
        t_error result;
        result = hil_set_card_specific_options(m_handle, options_str, std::strlen(options_str));
        util::sleep(util::milliseconds(10));
        if (result == 0) {
            LOG(Verbose) << "Set " << get_name() << " options to: \"" << m_options.get_string() << "\"";
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " options to: \"" << m_options.get_string() << "\" " << get_quanser_error_message(result);
            return false;
        }
    }
    return true;
}

QuanserOptions QuanserDaq::get_options() const {
    return m_options;
}

bool QuanserDaq::valid() const {
    return m_handle != nullptr && hil_is_valid(m_handle);
}

std::string QuanserDaq::manufactuer() const {
    char buf[256];
    t_error result = hil_get_string_property(m_handle, PROPERTY_STRING_MANUFACTURER, buf, ARRAY_LENGTH(buf));
    if (result == 0)
        return std::string(buf);    
    LOG(Error) << "Failed to get HIL string property PROPERTY_STRING_MANUFACTURER " << get_quanser_error_message(result);
    return "";
}

std::string QuanserDaq::product_name() const {
    char buf[256];
    t_error result = hil_get_string_property(m_handle, PROPERTY_STRING_PRODUCT_NAME, buf, ARRAY_LENGTH(buf));
    if (result == 0)
        return std::string(buf);    
    LOG(Error) << "Failed to get HIL string property PROPERTY_STRING_PRODUCT_NAME " << get_quanser_error_message(result);
    return "";
}

std::string QuanserDaq::model_name() const {
    char buf[256];
    t_error result = hil_get_string_property(m_handle, PROPERTY_STRING_MODEL_NAME, buf, ARRAY_LENGTH(buf));
    if (result == 0)
        return std::string(buf);    
    LOG(Error) << "Failed to get HIL string property PROPERTY_STRING_MODEL_NAME " << get_quanser_error_message(result);
    return "";
}

std::string QuanserDaq::hil_version() {
    tag_version v;
    v.size = sizeof(v);
    t_error result = hil_get_version(&v);
    if (result == 0) 
        return fmt::format("{}.{}.{}",v.major, v.minor, v.release);
    LOG(Error) << "Failed to get HIL SDK version " << get_quanser_error_message(result);
    return "";
}

void QuanserDaq::config_read(QuanserAI* AI, QuanserDI* DI, QuanserEncoder* enc, QuanserOtherInput* other) {
    m_AI = AI;
    m_DI = DI;
    m_enc = enc;
}

void QuanserDaq::config_write(QuanserAO* AO, QuanserDO* DO, QuanserPwm* pwm, QuanserOtherOutput* other) {
    m_AO = AO;
    m_DO = DO;
    m_pwm = pwm;
}

bool QuanserDaq::on_open() {
    t_error result = hil_open(m_card_type, std::to_string(m_id).c_str(), &m_handle);
    util::sleep(milliseconds(10));
    if (result == 0) {
        // successful open
        if (!set_options(m_options)) {
            // options didn't take so close
            hil_close(m_handle);
            LOG(Error) << "Opened " << get_name() << " but automatically closing because specified options failed to take effect";
            return false;
        }
        return true;
    }
    LOG(Error) << "Failed to open " << get_name() << " " << get_quanser_error_message(result);
    return false;
}

bool QuanserDaq::on_close() {
    t_error result = hil_close(m_handle);
    if (result == 0) {
        return true;
    }
    else {
        LOG(Error) << get_quanser_error_message(result, true);
        return false;
    }
}

bool QuanserDaq::on_enable() {
    return true;
}

bool QuanserDaq::on_disable() {
    return true;
}

} // namespace daq
} // namespace mahi