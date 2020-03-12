#include <Mahi/Daq/Quanser/QuanserDaq.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <Mahi/Util/Print.hpp>
#include <hil.h>
#include <unordered_map>
#include <utility>

#include <Mahi/Daq/Quanser/QuanserAI.hpp>
#include <Mahi/Daq/Quanser/QuanserAO.hpp>
#include <Mahi/Daq/Quanser/QuanserDI.hpp>
#include <Mahi/Daq/Quanser/QuanserDO.hpp>
#include <Mahi/Daq/Quanser/QuanserPWM.hpp>
#include <Mahi/Daq/Quanser/QuanserEncoder.hpp>
#include <Mahi/Daq/Quanser/QuanserOther.hpp>

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
} // private namespace

/// Structure for holding potential Modules that can be read/written
/// using Quansers synced read/write operations. I'm using PIMPL idiom
/// so as to not pollute the public API or confuse the user.
struct QuanserDaq::ReadWriteImpl {
    QuanserAI* AI = nullptr;
    QuanserAO* AO = nullptr;
    QuanserDI* DI = nullptr;
    QuanserDO* DO = nullptr;
    QuanserEncoder* EN = nullptr;
    QuanserPwm* PW = nullptr;
    QuanserOtherInput* OI = nullptr;
    QuanserOtherOutput* OO = nullptr;
    bool synced_read = false;
    bool synced_write = false;
};

QuanserDaq::QuanserDaq(const char* card_type) :
    m_card_type(card_type),
    m_id(inc_id(card_type)),
    m_h(nullptr),
    m_rw(std::make_unique<QuanserDaq::ReadWriteImpl>())
{
    set_name(std::string(m_card_type) + "-" + std::to_string(m_id));
}

QuanserDaq::~QuanserDaq() {
    dec_id(m_card_type);
}

bool QuanserDaq::read_all() {
    if (m_rw->synced_read) {
        // check: 1) not nullptr, 2) has more than 0 channels, and 3) client wants it to be read with read_all
        const bool read_AI = (m_rw->AI != nullptr && m_rw->AI->channels_internal().size() > 0 && m_rw->AI->read_with_all );
        const bool read_EN = (m_rw->EN != nullptr && m_rw->EN->channels_internal().size() > 0 && m_rw->EN->read_with_all );
        const bool read_DI = (m_rw->DI != nullptr && m_rw->DI->channels_internal().size() > 0 && m_rw->DI->read_with_all );;
        const bool read_OI = (m_rw->OI != nullptr && m_rw->OI->channels_internal().size() > 0 && m_rw->OI->read_with_all );
        auto result = hil_read(m_h, 
            read_AI ? &m_rw->AI->channels_internal()[0] : nullptr,                     // analog channels
            read_AI ? static_cast<t_uint32>(m_rw->AI->channels_internal().size()) : 0, // num analog channels 
            read_EN ? &m_rw->EN->channels_internal()[0] : nullptr,                     // encoder channels
            read_EN ? static_cast<t_uint32>(m_rw->EN->channels_internal().size()) : 0, // num encoder channels    
            read_DI ? &m_rw->DI->channels_internal()[0] : nullptr,                     // digital channels
            read_DI ? static_cast<t_uint32>(m_rw->DI->channels_internal().size()) : 0, // num digital channels 
            read_OI ? &m_rw->OI->channels_internal()[0] : nullptr,                     // other channels
            read_OI ? static_cast<t_uint32>(m_rw->OI->channels_internal().size()) : 0, // num other channels 
            read_AI ? &m_rw->AI->buffer()[0] : nullptr,                                // analog buffer
            read_EN ? &m_rw->EN->buffer()[0] : nullptr,                                // encoder buffer
            read_DI ? &m_rw->DI->buffer()[0] : nullptr,                                // digital buffer
            read_OI ? &m_rw->OI->buffer()[0] : nullptr                                 // other buffer
        );
        if (result == 0) {
            // call post read callbacks
            if (read_AI) { m_rw->AI->post_read.emit(&m_rw->AI->channels_internal()[0], &m_rw->AI->buffer()[0], m_rw->AI->channels_internal().size()); }
            if (read_EN) { m_rw->EN->post_read.emit(&m_rw->EN->channels_internal()[0], &m_rw->EN->buffer()[0], m_rw->EN->channels_internal().size()); }
            if (read_DI) { m_rw->DI->post_read.emit(&m_rw->DI->channels_internal()[0], &m_rw->DI->buffer()[0], m_rw->DI->channels_internal().size()); }
            if (read_OI) { m_rw->OI->post_read.emit(&m_rw->OI->channels_internal()[0], &m_rw->OI->buffer()[0], m_rw->OI->channels_internal().size()); }
            return true;
        }
        LOG(Error) << "Failed to read all inputs on " << name() << " " << quanser_msg(result);
        return false;
    }
    return Daq::read_all();
}

bool QuanserDaq::write_all() {
    if (m_rw->synced_write) {
        // check: 1) not nullptr, 2) has more than 0 channels, and 3) client wants it to be read with read_all
        const bool read_AO = (m_rw->AO != nullptr && m_rw->AO->channels_internal().size() > 0 && m_rw->AO->write_with_all );
        const bool read_PW = (m_rw->PW != nullptr && m_rw->PW->channels_internal().size() > 0 && m_rw->PW->write_with_all );
        const bool read_DO = (m_rw->DO != nullptr && m_rw->DO->channels_internal().size() > 0 && m_rw->DO->write_with_all );;
        const bool read_OO = (m_rw->OO != nullptr && m_rw->OO->channels_internal().size() > 0 && m_rw->OO->write_with_all );
        auto result = hil_write(m_h, 
            read_AO ? &m_rw->AO->channels_internal()[0] : nullptr,                     // analog channels
            read_AO ? static_cast<t_uint32>(m_rw->AO->channels_internal().size()) : 0, // num analog channels 
            read_PW ? &m_rw->PW->channels_internal()[0] : nullptr,                     // pwm channels
            read_PW ? static_cast<t_uint32>(m_rw->PW->channels_internal().size()) : 0, // num pwm channels    
            read_DO ? &m_rw->DO->channels_internal()[0] : nullptr,                     // digital channels
            read_DO ? static_cast<t_uint32>(m_rw->DO->channels_internal().size()) : 0, // num digital channels 
            read_OO ? &m_rw->OO->channels_internal()[0] : nullptr,                     // other channels
            read_OO ? static_cast<t_uint32>(m_rw->OO->channels_internal().size()) : 0, // num other channels 
            read_AO ? &m_rw->AO->buffer()[0] : nullptr,                                // analog buffer
            read_PW ? &m_rw->PW->buffer()[0] : nullptr,                                // pwm buffer
            read_DO ? &m_rw->DO->buffer()[0] : nullptr,                                // digital buffer
            read_OO ? &m_rw->OO->buffer()[0] : nullptr                                 // other buffer
        );
        if (result == 0) {
            // call post read callbacks
            if (read_AO) { m_rw->AO->post_write.emit(&m_rw->AO->channels_internal()[0], &m_rw->AO->buffer()[0], m_rw->AO->channels_internal().size()); }
            if (read_PW) { m_rw->PW->post_write.emit(&m_rw->PW->channels_internal()[0], &m_rw->PW->buffer()[0], m_rw->PW->channels_internal().size()); }
            if (read_DO) { m_rw->DO->post_write.emit(&m_rw->DO->channels_internal()[0], &m_rw->DO->buffer()[0], m_rw->DO->channels_internal().size()); }
            if (read_OO) { m_rw->OO->post_write.emit(&m_rw->OO->channels_internal()[0], &m_rw->OO->buffer()[0], m_rw->OO->channels_internal().size()); }
            return true;
        }
        LOG(Error) << "Failed to write all outputs on " << name() << " " << quanser_msg(result);
        return false;
    }
    return Daq::write_all();
}

bool QuanserDaq::set_options(const QuanserOptions& options) {
    m_options = options;
    char options_str[4096];
    std::strcpy(options_str, m_options.str().c_str());
    if (valid()) {
        t_error result;
        result = hil_set_card_specific_options(m_h, options_str, std::strlen(options_str));
        util::sleep(util::milliseconds(10));
        if (result == 0) {
            LOG(Verbose) << "Set " << name() << " options to: \"" << m_options.str() << "\"";
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << name() << " options to: \"" << m_options.str() << "\" " << quanser_msg(result);
            return false;
        }
    }
    return true;
}

QuanserOptions QuanserDaq::get_options() const {
    return m_options;
}

bool QuanserDaq::valid() const {
    return m_h != nullptr && hil_is_valid(m_h);
}

std::string QuanserDaq::manufactuer() const {
    char buf[256];
    t_error result = hil_get_string_property(m_h, PROPERTY_STRING_MANUFACTURER, buf, ARRAY_LENGTH(buf));
    if (result == 0)
        return std::string(buf);    
    LOG(Error) << "Failed to get HIL string property PROPERTY_STRING_MANUFACTURER " << quanser_msg(result);
    return "";
}

std::string QuanserDaq::product_name() const {
    char buf[256];
    t_error result = hil_get_string_property(m_h, PROPERTY_STRING_PRODUCT_NAME, buf, ARRAY_LENGTH(buf));
    if (result == 0)
        return std::string(buf);    
    LOG(Error) << "Failed to get HIL string property PROPERTY_STRING_PRODUCT_NAME " << quanser_msg(result);
    return "";
}

std::string QuanserDaq::model_name() const {
    char buf[256];
    t_error result = hil_get_string_property(m_h, PROPERTY_STRING_MODEL_NAME, buf, ARRAY_LENGTH(buf));
    if (result == 0)
        return std::string(buf);    
    LOG(Error) << "Failed to get HIL string property PROPERTY_STRING_MODEL_NAME " << quanser_msg(result);
    return "";
}

std::string QuanserDaq::serial_number() const {
    char buf[256];
    t_error result = hil_get_string_property(m_h, PROPERTY_STRING_SERIAL_NUMBER, buf, ARRAY_LENGTH(buf));
    if (result == 0)
        return std::string(buf);    
    LOG(Error) << "Failed to get HIL string property PROPERTY_STRING_SERIAL_NUMBER " << quanser_msg(result);
    return "";
}

std::string QuanserDaq::hil_version() {
    tag_version v;
    v.size = sizeof(v);
    t_error result = hil_get_version(&v);
    if (result == 0) 
        return fmt::format("{}.{}.{}",v.major, v.minor, v.release);
    LOG(Error) << "Failed to get HIL SDK version " << quanser_msg(result);
    return "";
}

void QuanserDaq::config_read(QuanserAI* AI, QuanserDI* DI, QuanserEncoder* EN, QuanserOtherInput* OI) {
    m_rw->synced_read = true;
    m_rw->AI = AI;
    m_rw->DI = DI;
    m_rw->EN = EN;
    m_rw->OI = OI;
}

void QuanserDaq::config_write(QuanserAO* AO, QuanserDO* DO, QuanserPwm* PW, QuanserOtherOutput* OO) {
    m_rw->synced_write = true;
    m_rw->AO = AO;
    m_rw->DO = DO;
    m_rw->PW = PW;
    m_rw->OO = OO;
}

bool QuanserDaq::on_daq_open() {
    t_error result = hil_open(m_card_type, std::to_string(m_id).c_str(), &m_h);
    util::sleep(milliseconds(10));
    if (result == 0) {
        // successful open
        if (!set_options(m_options)) {
            // options didn't take so close
            hil_close(m_h);
            LOG(Error) << "Opened " << name() << " but automatically closing because specified options failed to take effect.";
            return false;
        }
        return true;
    }
    LOG(Error) << "Failed to open " << name() << " " << quanser_msg(result);
    return false;
}

bool QuanserDaq::on_daq_close() {
    t_error result = hil_close(m_h);
    if (result == 0) {
        return true;
    }
    else {
        LOG(Error) << quanser_msg(result, true);
        return false;
    }
}

} // namespace daq
} // namespace mahi