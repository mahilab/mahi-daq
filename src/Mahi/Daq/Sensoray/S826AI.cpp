#include <Mahi/Daq/Sensoray/S826AI.hpp>
#include <Mahi/Daq/Sensoray/S826.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <windows.h>
#include <826api.h> 
#include <bitset>
#include "SensorayUtils.hpp"

using namespace mahi::util;

namespace mahi {
namespace daq {

S826AI::S826AI(S826& s826, unsigned int board) :
    AIModule(s826, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}),
    settling_times(*this, util::microseconds(5)),
    m_board(board)
{
    // set name
    set_name(s826.name() + ".AI");
    // write impl
    auto read_impl = [this](const ChanNum* chs, Voltage* vals, std::size_t n) {
        // set slots to read
        std::bitset<32> slotlist_bits;
        for (int i = 0; i < n; ++i)
            slotlist_bits[chs[i]].flip();
        int result;
        int adc_buffer[16];
        do {
            uint slotlist = static_cast<uint>(slotlist_bits.to_ulong());
            result = S826_AdcRead(m_board, adc_buffer, NULL, &slotlist, 0); // note: tmax=0
        } while (result == S826_ERR_NOTREADY);
        if (result != S826_ERR_OK) {
            LOG(Error) << "Failed to read " << name() << sensoray_msg(result);
            return false;
        }
        for (int i = 0; i < n; ++i) {
            int slot = adc_buffer[chs[i]];
            std::bitset<16> bits(slot); // get first 16 bits which hold the measured value
            short int value = static_cast<short int>(bits.to_ulong()); // get signed 16-bit int value
            vals[i] = ( value + 32768 ) * 20.0 / 65535.0 - 10.0; // convert to voltage
        }
        return true;
    };
    on_read.connect(read_impl);
    // settling times write impl
    auto settle_write_impl = [this](const ChanNum* chs, const util::Time* vals, std::size_t n) {
        bool success = true;
        for (int i = 0; i < n; ++i) {
            unsigned int tsettle = static_cast<unsigned int>(vals[i].as_microseconds());
            int result = S826_AdcSlotConfigWrite(m_board, chs[i], chs[i], tsettle, S826_ADC_GAIN_1);
            if (result != S826_ERR_OK) {
                LOG(Error) << "Failed to write " << name() << " channel number " << chs[i] << " settling time to " << vals[i] << sensoray_msg(result);
                success = false;
            }
        }
        return success;
    };
    settling_times.on_write.connect(settle_write_impl);
    // set channel numbers
    set_channels({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
}

bool S826AI::on_daq_open() {
    int result;
    bool success = true;
    // set default settling times of 5 microseconds
    if (settling_times.write(std::vector<util::Time>(16, 5_us)))
        success = false;
    // configure ADC slotlist
    result = S826_AdcSlotlistWrite(m_board, 0xFFFF, S826_BITWRITE);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to write slotlist of " << name() << " " << sensoray_msg(result);
        success = false;
    }
    // select free-running mode
    result = S826_AdcTrigModeWrite(m_board, 0);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to set ADC trigger mode of " << name() << " " << sensoray_msg(result);
        success = false;
    }
    // start conversions
    result = S826_AdcEnableWrite(m_board, 1);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to enable ADC conversions of " << name() << " " << sensoray_msg(result);
        success = false;
    }
    return success;
}
    
} // namespace daq
} // namespace mahi
