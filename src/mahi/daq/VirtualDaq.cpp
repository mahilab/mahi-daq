#include <mahi/daq/VirtualDaq.hpp>
#include <chrono>
#include <cmath>

namespace mahi {
namespace daq {

//==============================================================================
// VIRTUAL AI
//==============================================================================

static Voltage DEFAULT_AI_SOURCE(double t) {
    return std::sin(2.0 * 3.14159265358979 * t);
}

VirtualAI::VirtualAI(VirtualDaq& daq, const ChanNums& channel_numbers) :
    AnalogInput(channel_numbers),
    sources(this, DEFAULT_AI_SOURCE),
    daq_(daq)
{
    set_name(daq.get_name() + "_AI");
}

bool VirtualAI::update_channel(ChanNum channel_number) {
    double t = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    values_[channel_number] = sources[channel_number](t);
    return true;
}

//==============================================================================
// VIRUTAL AO
//==============================================================================

VirtualAO::VirtualAO(VirtualDaq& daq, const ChanNums& channel_numbers) : 
    AnalogOutput(channel_numbers),
    daq_(daq) 
{
    set_name(daq.get_name() + "_AO");
}

bool VirtualAO::update_channel(ChanNum channel_number) {
    return true;
}

//==============================================================================
// VIRTUAL DI
//==============================================================================

static Logic DEFAULT_DI_SOURCE(double t) {
    if (sin(2.0 * 3.14159265358979 * t) > 0.0)
        return High;
    else
        return Low;
}

VirtualDI::VirtualDI(VirtualDaq& daq, const ChanNums& channel_numbers) :
    DigitalInput(channel_numbers),
    sources(this, DEFAULT_DI_SOURCE),
    daq_(daq)
{
    set_name(daq.get_name() + "_DI");
}

bool VirtualDI::update_channel(ChanNum channel_number) {
    double t = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    values_[channel_number] = sources[channel_number](t);
    return true;
}

//==============================================================================
// VIRTUAL DO
//==============================================================================

VirtualDO::VirtualDO(VirtualDaq& daq, const ChanNums& channel_numbers) : 
    DigitalOutput(channel_numbers),
    daq_(daq)
{
    set_name(daq.get_name() + "_DO");
}

bool VirtualDO::update_channel(ChanNum channel_number) {
    return true;
}

//==============================================================================
// VIRTUAL ENCODER
//==============================================================================

int DEFAULT_ENCODER_SOURCE(double t) {
    return static_cast<int>(1024.0 * (sin(2.0 * 3.14159265358979 * t)));
}

VirtualEncoder::VirtualEncoder(VirtualDaq& daq, const ChanNums& channel_numbers) :
    Encoder(channel_numbers),
    sources(this, DEFAULT_ENCODER_SOURCE),
    daq_(daq)
{
    set_name(daq.get_name() + "_encoder");
}

bool VirtualEncoder::update_channel(ChanNum channel_number) {
    double t = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    values_[channel_number] = sources[channel_number](t);
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
