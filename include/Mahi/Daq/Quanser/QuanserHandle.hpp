#pragma once

/// Handle for Quanser devices. Must be in global namespace.
typedef struct tag_card* t_card;

namespace mahi {
namespace daq {

/// Typedef for Quanser device handle
typedef t_card QuanserHandle;

} // namespace daq
} // namespace mahi