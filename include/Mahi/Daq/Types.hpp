#pragma once
#include <string>
#include <typeinfo>
#include <vector>
#include <map>
#include <set>

namespace mahi {
namespace daq {

/// Default type for a channel number
typedef unsigned int ChanNum;

/// An array of channel numbers
typedef std::vector<ChanNum> ChanNums;

/// Maps a channel number to an array index
typedef std::map<unsigned int, std::size_t> ChanMap;

/// Represents a voltage in [V]
typedef double Voltage;

/// Represents a digital TTL logic level 
typedef char Logic;

/// Low TTL level, i.e. 0 V
constexpr Logic LOW  = 0;

/// High TTL, i.e. 3.3 V or 5 V
constexpr Logic HIGH = 1; 

/// Represents a digital pin direction
enum Direction {
    In,  ///< Digital input
    Out  ///< Digital output
};

/// Represents incremental encoder counts
typedef int Counts;

/// Represents incremental encoder quadrature mode
enum QuadMode {
    X0   = 0,  ///< no quadrature factor
    X1   = 1,  ///< 1X counts per revolution
    X2   = 2,  ///< 2X counts per revolution
    X4   = 4,  ///< 4X counts per revolution
};

/// Represents a duty cycle (e.g 0.5 = 50%)
typedef double DutyCycle;

} // namespace daq
} // namespace mahi