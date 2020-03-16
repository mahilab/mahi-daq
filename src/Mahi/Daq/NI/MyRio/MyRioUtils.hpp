#pragma once
#include <string>
#include <vector>
#include <bitset>

namespace mahi {
namespace daq {

/// Set a myRIO register bit to 1
void set_register_bit(unsigned int reg, int i);

/// Clear a myRIO register bit to 0
void clr_register_bit(unsigned int reg, int i);

/// Get a myRIO reigster bit
bool get_register_bit(unsigned int reg, int i);

/// Write a whole register from a bitset
void write_register(unsigned int reg, std::bitset<8> bits);

/// Write a whole register from a unsigned char
void write_register(unsigned int reg, unsigned char bits);

/// Read a whole register into a bitset
std::bitset<8> read_register(unsigned int reg);

/// Formats NI Error
std::string get_nifpga_error_message(int error);

//=============================================================================
// REGISTERS
//=============================================================================

// DIO

extern std::vector<uint32_t> SYSSELECT;
extern std::vector<std::vector<uint32_t>> DIRS;
extern std::vector<std::vector<uint32_t>> INS;
extern std::vector<std::vector<uint32_t>> OUTS;

// Encoder

extern std::vector<std::vector<int>>      ENC_BITS; 
extern std::vector<std::vector<uint32_t>> ENC_STAT;
extern std::vector<std::vector<uint32_t>> ENC_CNTR;
extern std::vector<std::vector<uint32_t>> ENC_CNFG;

// AO

extern std::vector<std::vector<uint32_t>> AO_REGISTERS;
extern std::vector<std::vector<double>>   AO_WEIGHTS;
extern std::vector<std::vector<double>>   AO_OFFSETS;

// AI

extern std::vector<std::vector<uint32_t>> AI_REGISTERS;
extern std::vector<std::vector<double>>   AI_WEIGHTS;
extern std::vector<std::vector<double>>   AI_OFFSETS;

} // namespace daq
} // namespace mahi
