#include <vector>
#include <Mahi/Util.hpp>
#include <Mahi/Daq.hpp>
#include <type_traits>

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
    MahiLogger->set_max_severity(Verbose);
    Q8Usb q8;    
  
    for (auto& m : q8.modules())
        print("{}", m->name());

    return 0;
}

// ModuleInterface []

// AO [], [=], write                 (RegistryW)
// AI [], read                       (RegistryR)
// AI.enable_values [],[=]           (BufferR)
// AI.expire_values write, [], write (RegistryW)