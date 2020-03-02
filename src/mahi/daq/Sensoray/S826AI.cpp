#include <mahi/daq/Sensoray/S826.hpp>
#include <mahi/daq/Sensoray/S826AI.hpp>
#include <windows.h>
#include <826api.h> 
#include <bitset>

#include "mahi/daq/LogImpl.hpp"

namespace mahi {
namespace daq {

S826AI::S826AI(S826& s826) : 
    AnalogInput({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}),
    s826_(s826)
{ 
    set_name(s826_.get_name() + "_AI");
}

bool S826AI::update() {
    int result;
    uint remaining = 0xFFFF; // all 16 timeslots
    do {
        uint slotlist = remaining;     // Read all available remaining timeslots.
        result = S826_AdcRead(s826_.board_, adc_buffer_, NULL, &slotlist, 0); // note: tmax=0
        remaining &= ~slotlist;
    } while (result == S826_ERR_NOTREADY);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to update " << get_name() << " (" << S826::get_error_message(result) << ")";
        return false;
    }
    // convert adc buffer to voltages
    for (auto& c : get_channel_numbers()) {
        int slot = adc_buffer_[c];
        std::bitset<16> bits(slot); // get first 16 bits which hold the measured value
        short int value = static_cast<short int>(bits.to_ulong()); // get signed 16-bit int value
        values_[c] = ( value + 32768 ) * 20.0 / 65535.0 - 10.0; // convert to voltage
    }
    return true;
}

bool S826AI::update_channel(ChanNum channel_number) {
    std::bitset<32> slotlist_bits;
    slotlist_bits[channel_number].flip();
    int result;
    do {
        ChanNum slotlist = slotlist_bits.to_ulong();
        result = S826_AdcRead(s826_.board_, adc_buffer_, NULL, &slotlist, 0); // note: tmax=0
    } while (result == S826_ERR_NOTREADY);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " (" << S826::get_error_message(result) << ")";
        return false;
    }
    int slot = adc_buffer_[channel_number];
    std::bitset<16> bits(slot); // get first 16 bits which hold the measured value
    short int value = static_cast<short int>(bits.to_ulong()); // get signed 16-bit int value
    values_[channel_number] = ( value + 32768 ) * 20.0 / 65535.0 - 10.0; // convert to voltage
    return true;
}

bool S826AI::set_settling_time(double t) {
    unsigned int tsettle = (unsigned int)(t * 1000000);
    // cofigure all timeslots for -10 to +10 V
    bool success = true;
    for (auto& c : get_channel_numbers()) {
        int result = S826_AdcSlotConfigWrite(s826_.board_, c, c, tsettle, S826_ADC_GAIN_1);
        if (result != S826_ERR_OK) {
            LOG(Error) << "Failed to set " << get_name() << " channel number " << c << " input range (" << S826::get_error_message(result) << ")";
            success = false;
        }
    }
    return success;
}


bool S826AI::on_open() {
    int result;
    bool success = true;
    if (!set_settling_time(5.0 / 1000000.0))
        success = false;
    // configure ADC slotlist
    result = S826_AdcSlotlistWrite(s826_.board_, 0xFFFF, S826_BITWRITE);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to write slotlist of " << get_name() << " (" << S826::get_error_message(result) << ")";
        success = false;
    }
    // select free-running mode
    result = S826_AdcTrigModeWrite(s826_.board_, 0);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to set ADC trigger mode of " << get_name() << " (" << S826::get_error_message(result) << ")";
        success = false;
    }
    // start conversions
    result = S826_AdcEnableWrite(s826_.board_, 1);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to enable ADC conversions of " << get_name() << " (" << S826::get_error_message(result) << ")";
        success = false;
    }
    return success;
}

} // namespace daq
} // namespace mahi