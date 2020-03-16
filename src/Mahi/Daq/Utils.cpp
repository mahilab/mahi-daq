#include <Mahi/Daq/Utils.hpp>
#include <Mahi/Daq/Module.hpp>
#include <Mahi/Daq/Buffer.hpp>
#include <Mahi/Util/Print.hpp>
#include <Mahi/Daq/Watchdog.hpp>
#include <Mahi/Daq/Io.hpp>

using namespace mahi::util;

namespace mahi {
namespace daq {

void print_info(const Daq& daq) {
    fmt::print("DAQ: {}\n", daq.name());
    fmt::print("Modules: {}\n", daq.modules().size());
    for (std::size_t i = 0; i < daq.modules().size(); ++i) {
        auto m = daq.modules()[i];
        print("{}. {}",i, m->name());
        if (auto cm = dynamic_cast<ChanneledModule*>(m)) {
            std::string type = "Other";
            if (dynamic_cast<AIModule*>(m))
                type = "Analog Input";
            if (dynamic_cast<AOModule*>(m))
                type = "Analog Output";
            if (dynamic_cast<DIModule*>(m))
                type = "Digital Input";
            if (dynamic_cast<DOModule*>(m))
                type = "Digital Output";
            if (dynamic_cast<EncoderModuleBasic*>(m))
                type = "Encoder";
            if (dynamic_cast<PwmModuleBasic*>(m))
                type = "PWM";
            fmt::print("   Type:             {}\n",type);
            fmt::print("   Allowed Channels: [{}]\n", fmt::join(cm->channels_allowed(),", "));
            fmt::print("   Current Channels: [{}]\n", fmt::join(cm->channels(),", "));
            fmt::print("   Readable:         {}\n", dynamic_cast<Readable*>(cm) != nullptr ? "Yes" : "No");
            fmt::print("   Writeable:        {}\n", dynamic_cast<Writeable*>(cm) != nullptr ? "Yes" : "No");
            fmt::print("   Shares Pins:      {}\n",cm->shares_pins() ? "Yes" : "No");
        }
        if (auto wm = dynamic_cast<Watchdog*>(m)) {
            fmt::print("   Current Timeout:  {} ms\n", wm->timeout().as_milliseconds());
        }
    }
}

} // namespace daq
} // namespace mahi