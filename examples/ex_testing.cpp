#include <vector>
#include <Mahi/Util.hpp>
#include <Mahi/Daq.hpp>

using namespace mahi::util;
using namespace mahi::daq;


class CurrentAmplifier {
public:
    CurrentAmplifier(double& command_voltage, const double& sense_voltage, double gain) : 
        m_command_voltage(command_voltage), 
        m_sense_voltage(sense_voltage),
        m_gain(gain) 
    { }
    void command(double amps) {
        m_command_voltage = amps / m_gain;
    }
    double sense() {
        return m_gain * m_sense_voltage;
    }
    double& m_command_voltage;
    const double& m_sense_voltage;
    double m_gain; // A/V
};


class AIMod : public ModuleBase, public BufferW<double, AIMod> {
public:
    AIMod() : 
        BufferW<double, AIMod>(this),
        minValues(this, -10),
        maxValues(this, 10) 
    { 
        set_channel_numbers({0,1,2,3,4});
    }

    BufferW<double, AIMod> minValues;
    BufferW<double, AIMod> maxValues;
};

class MyModule : public Module<double> {
public:
    MyModule() : AI(this), AO(this) { 

        set_channel_numbers({0,1,2,3,4});
        set_name("MyModule");

        AI.on_read_one = [](ChanNum ch, double& val) {
            val = 4;
            print("Reading channel {} value = {}", ch, val);
            return true;
        };

        AI.on_read_all = [](const ChanNums& chs, std::vector<double>& vals) {
            vals = {1,1,1,1,1};
            print("Reading channels {} values = {}", chs, vals);
            return true;
        };

        AO.on_write_one = [](ChanNum ch, double val) {
            print("Writing channel {} value = {}", ch, val);
            return true;
        };

        AO.on_write_all = [](const ChanNums& chs, const std::vector<double>& vals) {
            print("Writing channels {} values = {}", chs, vals);
            return true;
        };
    }

    BufferR<double, MyModule> AI;
    BufferW<double, MyModule> AO;
};

int main(int argc, char const *argv[])
{
    MyModule myMod;

    CurrentAmplifier amp(myMod.AO[0], myMod.AI[0], 0.2);

    print("Ch. Count:  {}",myMod.channel_count());
    print("Ch. Nums:   {}",myMod.channel_numbers());
    print("Read Size:  {}",myMod.AI.size());
    print("Write Size: {}",myMod.AO.size());
    print("AI:         {}",myMod.AI);
    print("AO:         {}",myMod.AO);
    myMod.AI.read();
    myMod.AI.read(4);
    print("AI:         {}",myMod.AI);

    myMod.AO[0] = 14;
    print("AO:         {}",myMod.AO);
    myMod.AO.write();
    myMod.AO.write(4, 20);
    print("AO:         {}",myMod.AO);
    myMod.AO.write({1,2,3,4,5});
    print("AO:         {}",myMod.AO);


    // get the current sense from the amplifier
    print("Sense:    {} A", amp.sense());
    // set the amplifier command current
    amp.command(0.25);
    print("Command:  {} A", 0.25);
    myMod.AO.write();

    AIMod aimod;
    print("{}",aimod.channel_numbers());
    aimod[0] = 4;
    aimod.write();
    print("{}",aimod[0]);
    aimod.minValues[0] = 1;
    aimod.minValues.write();
    print("The End");
}
