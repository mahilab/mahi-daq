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

#include <Mahi/Daq.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::daq;
using namespace mahi::util;

/// prints the current myRIO channel configuration
// void print_channel_info(const MyRio& myrio) {
//     print("----------------------------------------");
//     print("Connector MXP A");
//     print("    AO: ", myrio.mxpA.AO.channels_internal());
//     print("    AI: ", myrio.mxpA.AI.channels_internal());
//     print("    DI: ", myrio.mxpA.DIO.input_channel_numbers());
//     print("    DO: ", myrio.mxpA.DIO.output_channel_numbers());
//     print("    ENC:", myrio.mxpA.encoder.channels_internal());
//     print("Connector MXP B");
//     print("    AO: ", myrio.mxpB.AO.channels_internal());
//     print("    AI: ", myrio.mxpB.AI.channels_internal());
//     print("    DI: ", myrio.mxpB.DIO.input_channel_numbers());
//     print("    DO: ", myrio.mxpB.DIO.output_channel_numbers());
//     print("    ENC:", myrio.mxpB.encoder.channels_internal());
//     print("Connector MSP C");
//     print("    AO: ", myrio.mspC.AO.channels_internal());
//     print("    AI: ", myrio.mspC.AI.channels_internal());
//     print("    DI: ", myrio.mspC.DIO.input_channel_numbers());
//     print("    DO: ", myrio.mspC.DIO.output_channel_numbers());  
//     print("    ENC:", myrio.mspC.encoder.channels_internal());
//     print("----------------------------------------");
// }

// main
int main(int argc, char** argv) {
    MahiLogger->set_max_severity(Verbose);

    MyRio myrio;
    myrio.mspC.encoder.set_channels({0,1});
    myrio.mxpA.encoder.set_channels({0});
    myrio.mxpB.encoder.set_channels({0});

    myrio.mspC.DO.set_channels({0,1,2,3,4,5,6,7});

    // for (int i = 0; i < 500; ++i) {
    //     myrio.read_all();
    //     print("{}",myrio.mxpA.AI[0]);
    //     double out = 2.5 + 2.5 * std::sin(TWOPI * i * 0.01);
    //     myrio.mxpB.AO[0] = out;        
    //     myrio.write_all();
    //     sleep(10_ms);
    // }
 
}
