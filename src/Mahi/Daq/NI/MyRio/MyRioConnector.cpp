#include <Mahi/Daq/NI/MyRio/MyRioConnector.hpp>
#include <Mahi/Daq/NI/MyRio/MyRio.hpp>
#include "MyRioUtils.hpp"
#include "Detail/MyRioFpga60/MyRio.h"

// MXP Shared Pins
// I2C[0] = DIO[14:15]
// ENC[0] = DIO[11:12]
// PWM[0] = DIO[8]
// PWM[1] = DIO[9]
// PWM[2] = DIO[10]
// SPI[0] = DIO[5:7]

// MSP Shared Pins
// PWM[0] = DIO[3]
// PWM[1] = DIO[7]
// ENC[0] = DIO[0,2]
// ENC[1] = DIO[4,6]

namespace mahi {
namespace daq {

namespace {
std::string connectorName(MyRioConnector::Type type) {
    if (type == MyRioConnector::Type::MxpA)
        return "MxpA";
    else if (type == MyRioConnector::Type::MxpB)
        return "MxpB";
    else if (type == MyRioConnector::Type::MspC)
        return "MspC";
    else
        return "Unkown";
}
} // namespace
    
MyRioConnector::MyRioConnector(MyRio& myrio, Type _type) :
    Daq(myrio.name() + "." + connectorName(_type)),
    type(_type), m_myrio(myrio)
{ }

MyRioMxp::MyRioMxp(MyRio& myrio, Type type) :
    MyRioConnector(myrio, type),
    AI(*this, {0,1,2,3}),
    AO(*this, {0,1}),
    DI(*this, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}),
    DO(*this, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}),
    encoder(*this, {0})
{
    // pin sharing
    SharedPins list(16);
    for (ChanNum i = 0; i < 16; ++i)
        list[i] = {{i},{i}};
    create_shared_pins(&DI, &DO,list);
    create_shared_pins(&encoder, &DO, {{{0},{11,12}}});
    create_shared_pins(&encoder, &DI, {{{0},{11,12}}});
}

bool MyRioMxp::on_daq_open() {
    auto bits = read_register(SYSSELECT[type]);
    ChanNums dio_chs, di_chs, do_chs, enc_chs, pwm_chs, i2c_chs, spi_chs;

    if (bits[7]) 
        i2c_chs.push_back(0);
    else 
        dio_chs.insert(dio_chs.end(), {14,15});

    if (bits[5])
        enc_chs.push_back(0);
    else
        dio_chs.insert(dio_chs.end(), {11,12});
    
    if (bits[4])
        pwm_chs.push_back(2);
    else
        dio_chs.push_back(10);

    if (bits[3])
        pwm_chs.push_back(1);
    else
        dio_chs.push_back(9);

    if (bits[2])
        pwm_chs.push_back(0);
    else
        dio_chs.push_back(8);

    if (bits[1] || bits[0])
        spi_chs.push_back(0);
    if (bits[1] && !bits[0]) 
        dio_chs.push_back(6);    
    else if (!bits[1] && bits[0]) 
        dio_chs.push_back(5);
    else 
        dio_chs.insert(dio_chs.end(), {5,6,7});
    
    dio_chs.insert(dio_chs.end(), {0,1,2,3,4,13});

    for (auto& c : dio_chs) {
        if (get_bit(DIRS[type][c / 8], c % 8))
            do_chs.push_back(c);
        else
            di_chs.push_back(c);
    }

    AI.set_channels({0,1,2,3});
    AO.set_channels({0,1});
    DI.set_channels(di_chs);
    DO.set_channels(do_chs);
    encoder.set_channels(enc_chs);
    // TODO: others, when implemented

    return true;
}

MyRioMsp::MyRioMsp(MyRio& myrio, Type type) :
    MyRioConnector(myrio, type),
    AI(*this, {0,1}),
    AO(*this, {0,1}),
    DI(*this, {0,1,2,3,4,5,6,7}),
    DO(*this, {0,1,2,3,4,5,6,7}),
    encoder(*this, {0,1})
{
    // pin sharing
    SharedPins list(8);
    for (ChanNum i = 0; i < 8; ++i)
        list[i] = {{i},{i}};
    create_shared_pins(&DI, &DO,list);
    create_shared_pins(&encoder, &DO, {{{0},{0,2}},{{1},{4,6}}});
    create_shared_pins(&encoder, &DI, {{{0},{0,2}},{{1},{4,6}}});
}

bool MyRioMsp::on_daq_open() {
    auto bits = read_register(SYSSELECT[type]);
    ChanNums dio_chs, di_chs, do_chs, enc_chs, pwm_chs;

    if (bits[3])
        pwm_chs.push_back(1);
    else
        dio_chs.push_back(7);
    
    if (bits[1])
        pwm_chs.push_back(0);
    else
        dio_chs.push_back(3);

    if (bits[2])
        enc_chs.push_back(1);
    else
        dio_chs.insert(dio_chs.end(), {4,6});

    if (bits[0])
        enc_chs.push_back(0);
    else
        dio_chs.insert(dio_chs.end(), {2,0});

    dio_chs.insert(dio_chs.end(), {1,5});

    for (auto& c : dio_chs) {
        if (get_bit(DIRS[type][c / 8], c % 8))
            do_chs.push_back(c);
        else
            di_chs.push_back(c);
    }

    AI.set_channels({0,1});
    AO.set_channels({0,1});
    DI.set_channels(di_chs);
    DO.set_channels(do_chs);
    encoder.set_channels(enc_chs);
    // TODO: others, when implemented

    return true;
}

/*

void MyRioConnector::reset() {
    for (auto& ch : encoder.channels_internal())
        encoder.disable_channel(ch);
    DIO.sync();
}


*/

} // namespace daq
} // namespace mahi