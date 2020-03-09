#include <vector>
#include <Mahi/Util.hpp>
#include <Mahi/Daq.hpp>

using namespace mahi::util;
using namespace mahi::daq;

// class VirtualEncoder : public EncoderModule<void> {
// public:
//     VirtualEncoder(const ChanNums& channels) {
//         set_channel_numbers(channels);
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
    Printer(const double& pos) : pos(pos) { }
    void print() {
        ::print("Pos: {} revolutions", pos);
    }
    const double& pos;
};

class Stuff {
protected:
    int x;
};

void read(Readable* readable) {
    print("Reading");
    readable->read();
};

int main(int argc, char const *argv[])
{
    // VirtualEncoder enc({0,1,2,3});

    // for (auto& ch : enc.channel_numbers())
    //     enc.units[ch] = 1.0 / 1024.0;
    // print("Counts:    {}",enc);
    // print("Positions: {}",enc.positions);
    // print("Quads:     {}",enc.quadratures);
    // enc.read();
    // print("Counts:    {}",enc);
    // print("Positions: {}",enc.positions);
    // enc.write({40,40,40,40});
    // print("Counts:    {}",enc);
    // enc.zero(3);
    // print("Counts:    {}",enc);
    // enc.zero();
    // print("Counts:    {}",enc);

    // Printer printer(enc.positions[0]);

    Q8Usb q8;


    // q8.open();
    // q8.set_options(QuanserOptions());
    // q8.AO.min_values;
    // q8.AO.write();
    // q8.AO.write(1,0);
    // q8.AO.write({0,1,2,3,4,5,6,7});
    // q8.AI.read();
    // q8.AI.read(0);
    // print("hey!");
    return 0;
}

// ModuleInterface []

// AO [], [=], write                 (RegistryW)
// AI [], read                       (RegistryR)
// AI.enable_values [],[=]           (BufferR)
// AI.expire_values write, [], write (RegistryW)