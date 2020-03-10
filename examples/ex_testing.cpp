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


int main(int argc, char const *argv[])
{
    MahiLogger->set_max_severity(Verbose);
    // Q8Usb q8;   
    // q8.open(); 
    // q8.enable();
    // print("Manufacturer:  {}",q8.manufactuer());
    // print("Product Name:  {}",q8.product_name());
    // print("Model Name:    {}",q8.model_name());
    // print("Serial Number: {}",q8.serial_number());

    // q8.watchdog.set_timeout(50_ms);
    // for (int i = 0; i < 1000; ++i)
    // {
    //     print("{}", (int)q8.DI[0]);
    //     print("{}",q8.AI.get());
    //     q8.AO[0] = sin(TWOPI*i*0.01);
    //     q8.DO[0] = 1;
    //     q8.write_all();
    //     if (i == 200)
    //         sleep(100_ms);
    //     sleep(10_ms);
    // }

    // q8.open();
    // q8.enable();

    Daq daq;
    DOModule d(daq); d.set_name("d");
    EncoderModule<> e(daq); e.set_name("e");
    ChannelsModule::share(&e,&d,{{{0,1},{0}},{{2,3},{1}}});
    d.set_channel_numbers({0,1,2,3});
    e.set_channel_numbers({0,1});



    return 0;
}

// ModuleInterface []

// AO [], [=], write                 (RegistryW)
// AI [], read                       (RegistryR)
// AI.enable_values [],[=]           (BufferR)
// AI.expire_values write, [], write (RegistryW)