#include <Mahi/Daq/VirtualDaq.hpp>
#include <Mahi/Util/Timing/Clock.hpp>
#include <Mahi/Util/Math/Constants.hpp>
#include <cmath>

namespace mahi {
namespace daq {

namespace {
    static Voltage default_ai_source() {
        static util::Clock clk;
        return std::sin(2.0 * util::TWOPI * clk.get_elapsed_time().as_seconds());
    }

    static void default_ao_sink(Voltage v) {
        v = v;
    }

    static Logic default_di_source() {
        static util::Clock clk;
        if (sin(2.0 * 3.14159265358979 * clk.get_elapsed_time().as_seconds()) > 0.0)
            return High;
        else
            return Low;
    }

    static void default_do_sink(Logic logic) {
        logic = logic;
    }

    int default_encoder_source() {
        static util::Clock clk;
        return static_cast<int>(1024.0 * (sin(2.0 * 3.14159265358979 * clk.get_elapsed_time().as_seconds())));
    }
}

//==============================================================================
// VIRTUAL AI
//==============================================================================

VirtualAI::VirtualAI(VirtualDaq& daq, const ChanNums& channel_numbers) :
    AnalogInput(channel_numbers),
    sources(this, default_ai_source),
    daq_(daq)
{
    set_name(daq.get_name() + "_AI");
}

bool VirtualAI::update_channel(ChanNum channel_number) {
    m_values[channel_number] = sources[channel_number]();
    return true;
}

//==============================================================================
// VIRUTAL AO
//==============================================================================

VirtualAO::VirtualAO(VirtualDaq& daq, const ChanNums& channel_numbers) : 
    AnalogOutput(channel_numbers),
    sinks(this, default_ao_sink),
    daq_(daq) 
{
    set_name(daq.get_name() + "_AO");
}

bool VirtualAO::update_channel(ChanNum channel_number) {
    sinks[channel_number](m_values[channel_number]);
    return true;
}

//==============================================================================
// VIRTUAL DI
//==============================================================================


VirtualDI::VirtualDI(VirtualDaq& daq, const ChanNums& channel_numbers) :
    DigitalInput(channel_numbers),
    sources(this, default_di_source),
    daq_(daq)
{
    set_name(daq.get_name() + "_DI");
}

bool VirtualDI::update_channel(ChanNum channel_number) {
    m_values[channel_number] = sources[channel_number]();
    return true;
}

//==============================================================================
// VIRTUAL DO
//==============================================================================

VirtualDO::VirtualDO(VirtualDaq& daq, const ChanNums& channel_numbers) : 
    DigitalOutput(channel_numbers),
    sinks(this, default_do_sink),
    daq_(daq)
{
    set_name(daq.get_name() + "_DO");
}

bool VirtualDO::update_channel(ChanNum channel_number) {
    sinks[channel_number](m_values[channel_number]);
    return true;
}

//==============================================================================
// VIRTUAL ENCODER
//==============================================================================


VirtualEncoder::VirtualEncoder(VirtualDaq& daq, const ChanNums& channel_numbers) :
    Encoder(channel_numbers),
    sources(this, default_encoder_source),
    daq_(daq)
{
    set_name(daq.get_name() + "_encoder");
}

bool VirtualEncoder::update_channel(ChanNum channel_number) {
    m_values[channel_number] = sources[channel_number]();
    return true;
}

bool VirtualEncoder::reset_count(ChanNum channel_number, int count) {
    return true;
}


//==============================================================================
// VIRTUAL DAQ
//==============================================================================

VirtualDaq::VirtualDaq(const std::string& name)
    : DaqBase(name),
      AI(*this, {0,1,2,3,4,5,6,7,8}),
      AO(*this, {0,1,2,3,4,5,6,7,8}),
      DI(*this, {0,1,2,3,4,5,6,7,8}),
      DO(*this, {0,1,2,3,4,5,6,7,8}),
      encoder(*this, {0,1,2,3,4,5,6,7,8})
{

}

VirtualDaq::~VirtualDaq() {
    if (is_enabled())
        this->disable();
    if (is_open())
        this->close();
}

bool VirtualDaq::on_enable() {
    AI.enable();
    AO.enable();
    DI.enable();
    DO.enable();
    encoder.enable();
    return true;
}

bool VirtualDaq::on_disable() {
    AI.disable();
    AO.disable();
    DI.disable();
    DO.disable();
    encoder.disable();
    return true;
}

bool VirtualDaq::update_input() {
    AI.update();
    DI.update();
    encoder.update();
    return true;
}

bool VirtualDaq::update_output() {
    AO.update();
    DO.update();
    return true;
}

bool VirtualDaq::on_open() {
    return true;
}

bool VirtualDaq::on_close() {
    return true;
}

} // namespace daq
} // namespace mahi
