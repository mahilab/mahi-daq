#include <vector>
#include <Mahi/Util.hpp>
#include <Mahi/Daq.hpp>
#include <type_traits>

using namespace mahi::util;
using namespace mahi::daq;

// class VirtualEncoder : public EncoderModule<void> {
// public:
//     VirtualEncoder(const ChanNums& channels) {
//         set_channels(channels);
//         auto read_impl = [this](const ChanNum* chs, int* counts, std::size_t n) {
//             for (int i = 0; i < n; ++i)
//                 counts[i] += (i+1) * 128;
//             return true;
//         };
//         on_read.connect(read_impl);
//         auto write_impl = [this](const ChanNum* chs, const int* counts, std::size_t n) {
//             for (int i = 0; i < n; ++i)
//                 fmt::print("{},", counts[i]);
//             fmt::print("\n");
//             return true;
//         };
//         on_write.connect(write_impl);
//     }
// };

struct Printer {
    Printer(const double* pos) : pos(pos) { }
    void print() {
        ::print("Pos: {} deg", *pos);
    }
    const double* pos;
};

class Stuff {
protected:
    int x;
};

void read(Readable* readable) {
    print("Reading");
    readable->read();
};

// void share_test() {
//     Daq daq;

//     ChanneledModule DIO(daq, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
//     DIO.set_name("DIO");
//     ChanneledModule I2C(daq, {0});
//     I2C.set_name("I2C");
//     ChanneledModule ENC(daq, {0});
//     ENC.set_name("ENC");
//     ChanneledModule PWM(daq,{0,1,2});
//     PWM.set_name("PWM");
//     ChanneledModule SPI(daq, {0});
//     SPI.set_name("SPI");

//     DIO.share_pins_with(&I2C,{{{14,15},{0}}});
//     DIO.share_pins_with(&ENC,{{{11,12},{0}}});
//     DIO.share_pins_with(&PWM,{{{8},{0}},{{9},{1}},{{10},{2}}});
//     DIO.share_pins_with(&SPI,{{{5,6,7},{0}}});

//     DIO.set_channels({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
//     I2C.set_channels({0});
//     ENC.set_channels({0});
//     PWM.set_channels({0,1,2});
//     SPI.set_channels({0});
//     DIO.set_channels({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
// }

int main(int argc, char const *argv[])
{
    MahiLogger->set_max_severity(Verbose);

    Q2Usb q2;
    q2.DO.set_channels({4,5,6,7,8});
    q2.enable();
    q2.encoder.zero();
    for (int i = 0; i < 1000; ++i) {
        q2.read_all();
        print("ENC: {}",q2.encoder.converted[0]);
        q2.AO[0] = 10 * std::sin(TWOPI*i*0.01);
        q2.write_all();
        sleep(10_ms);
    }
    q2.read_all();
    q2.write_all();
    q2.disable();
    q2.AI;

    return 0;
}

