#include <Mahi/Daq/Quanser/QuanserOptions.hpp>

namespace mahi {
namespace daq {

std::string QuanserOptions::str() {
    std::string options = "";

    if (update_rate == UpdateRate::Fast)
        options += "update_rate=fast;";

    if (decimation != 1)
        options += "decimation=" + std::to_string(decimation) + ";";

    if (led == LedMode::Auto)
        options += "led=auto;";
    else if (led == LedMode::User)
        options += "led=user;";

    if (d0 == DoMode::Digital)
        options += "d0=digital;";
    else if (d0 ==DoMode::Pwm)
        options += "d0=pwm;";

    if (d1 == DoMode::Digital)
        options += "d1=digital;";
    else if (d1 ==DoMode::Pwm)
        options += "d1=pwm;";    

    for (auto& x : encX_dir) {
        if (x.second == EncoderDirection::Reversed)
            options += "enc" + std::to_string(x.first) + "_dir=1;";
    }

    for (auto& x : encX_filter) {
        if (x.second == EncoderFilter::Filtered)
            options += "enc" + std::to_string(x.first) + "_filter=1;";
    }

    for (auto& x : encX_a) {
        if (x.second == EncoderDetection::Low)
            options += "enc" + std::to_string(x.first) + "_a=1;";
    }

    for (auto& x : encX_b) {
        if (x.second == EncoderDetection::Low)
            options += "enc" + std::to_string(x.first) + "_b=1;";
    }

    for (auto& x : encX_z) {
        if (x.second == EncoderDetection::Low)
            options += "enc" + std::to_string(x.first) + "_z=1;";
    }

    for (auto& x : encX_reload) {
        if (x.second == EncoderReload::OnPulse)
            options += "enc" + std::to_string(x.first) + "_reload=1;";
    }

    for (auto& x : encX_velocity) {
        std::string v = std::to_string(x.second);
        v.resize(7);
        options += "enc" + std::to_string(x.first) + "_velocity=" + v + ";";
    }

    for (auto& x : pwmX_en) {
        if (x.second)
            options += "pwm" + std::to_string(x.first) + "_en=1;";
        else
            options += "pwm" + std::to_string(x.first) + "_en=0;";
    }


    for (auto& x : aoX_params) {
        std::string ch = "ch" + std::to_string(x.first) + "_";

        std::string mode_str = std::to_string(static_cast<int>(x.second.ch_mode));
        std::string kff_str  = std::to_string(x.second.ch_kff);     kff_str.resize(7);
        std::string a0_str   = std::to_string(x.second.ch_a0);      a0_str.resize(7);
        std::string a1_str   = std::to_string(x.second.ch_a1);      a1_str.resize(7);
        std::string a2_str   = std::to_string(x.second.ch_a2);      a2_str.resize(7);
        std::string b0_str   = std::to_string(x.second.ch_b0);      b0_str.resize(7);
        std::string b1_str   = std::to_string(x.second.ch_b1);      b1_str.resize(7);
        std::string post_str = std::to_string(x.second.ch_post);    post_str.resize(7);

        options += ch + "mode=" + mode_str + ";";
        options += ch + "kff="  + kff_str  + ";";
        options += ch + "a0="   + a0_str   + ";";
        options += ch + "a1="   + a1_str   + ";";
        options += ch + "a2="   + a2_str   + ";";
        options += ch + "b0="   + b0_str   + ";";
        options += ch + "b1="   + b1_str   + ";";
        options += ch + "post=" + post_str + ";";
    }

    if (special.length() > 0) {
        if (special[special.length()-1] != ';')
            special += ";";
        options += special;
    }
    
    return options;
}

void QuanserOptions::clear() {
    *this = QuanserOptions();
}

} // namespace daq
} // namespace mahi
