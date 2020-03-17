// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once
#include <Mahi/Daq/Daq.hpp>

#include <Mahi/Daq/NI/MyRio/MyRioAI.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioAO.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioDI.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioDO.hpp>
#include <Mahi/Daq/NI/MyRio/MyRioEncoder.hpp>

namespace mahi {
namespace daq {

class MyRio;

/// myRIO Connector Base Class
class MyRioConnector : public Daq {
public:
    enum Type { MxpA = 0, MxpB = 1, MspC = 2 };
    const Type type;      ///< connector Type
protected:
    friend class MyRio;
    MyRioConnector(MyRio& myrio, Type type);
    using Daq::open;
    using Daq::close;
    using Daq::enable;
    using Daq::disable;
protected:
    MyRio& m_myrio;  ///< reference to myRIO this connector is on
};

//==============================================================================
// MXP CONNECTORS
//==============================================================================

/// myRIO MXP Connector (i.e. MXPA, MXPB)
///
/// Pinout:
///
/// [33][31][29][27][25][23][21][19][17][15][13][11][09][07][05][03][01]
/// [34][32][30][28][26][24][22][20][18][16][14][12][10][08][06][04][02]
///
/// [01] = +5V
/// [02] = AO[0] -> AGND
/// [03] = AI[0] -> AGND
/// [04] = AO[1] -> AGND
/// [05] = AI[1] -> AGND
/// [06] = AGND
/// [07] = AI[2] -> AGND
/// [08] = DGND
/// [09] = AI[3] -> AGND
/// [10] = UART.RX
/// [11] = DI[0], DO[0]
/// [12] = DGND
/// [13] = DI[1], DO[1]
/// [14] = UART.TX
/// [15] = DI[2], DO[2]
/// [16] = DGND
/// [17] = DI[3] DO[3]
/// [18] = DI[11], DO[11], ENC[0].A
/// [19] = DI[4], DO[4]
/// [20] = DGND
/// [21] = DI[5], DO[5], SPI[0].CLK
/// [22] = DI[12], DO[12], ENC[0].B
/// [23] = DI[6], DO[6], SPI[0].MISO
/// [24] = DGND
/// [25] = DI[7], DO[7], SPI[0].MOSI
/// [26] = DI[13], DO[13]
/// [27] = DI[8], DO[8], PWM[0]
/// [28] = DGBD
/// [29] = DI[9], DO[9], PWM[1]
/// [30] = DGND
/// [31] = DI[10], DO[10], PWM[2]
/// [32] = DI[14], DO[14], I2C[0].SCL
/// [33] = +3.3V
/// [34] = DI[15], DO[15], I2C[0].SDA
class MyRioMxp : public MyRioConnector {
public:
    MyRioAI AI;
    MyRioAO AO;
    MyRioDI DI;
    MyRioDO DO;
    MyRioEncoder encoder;
protected:
    friend class MyRio;
    MyRioMxp(MyRio& myrio, Type type);
    bool on_daq_open() override;
};

//==============================================================================
// MSP CONNECTOR
//==============================================================================

/// myRIO MSP Connector (i.e. MSPC)
/// 
/// Pinout:
///
/// [01][02][03][04][05][06][07][08][09][10][11][12][13][14][15][16][17][18][19][20]
///
/// [01] = +15V -> AGND
/// [02] = -15V -> AGND
/// [03] = AGND
/// [04] = AO[0] -> AGND
/// [05] = AI[1] -> AGND
/// [06] = AGND
/// [07] = AI[0]+
/// [08] = AI[0]-
/// [09] = AI[1]+
/// [10] = AI[1]-
/// [11] = DI[0], DO[0], ENC[0].A
/// [12] = DI[1], DO[1]
/// [13] = DI[2], DO[2], ENC[0].B
/// [14] = DI[3], DO[3], PWM[0]
/// [15] = DI[4], DO[4], ENC[1].A
/// [16] = DI[5], DO[5]
/// [17] = DI[6], DO[6], ENC[1].B
/// [18] = DI[7], DO[7], PWM[1] 
/// [19] = DGND
/// [20] = 5V -> DGND
class MyRioMsp : public MyRioConnector {
public:
    MyRioAI AI;
    MyRioAO AO;
    MyRioDI DI;
    MyRioDO DO;
    MyRioEncoder encoder;
protected:
    friend class MyRio;
    MyRioMsp(MyRio& myrio, Type type);
    bool on_daq_open() override;
};

} // namespace daq
} // namespace mahi

