#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin.hpp"

// Initialize Pin::pin_map to LPC40xx memory mapped LPC_IOCON register
Pin::PinMap_t * Pin::pin_map = reinterpret_cast<PinMap_t *>(LPC_IOCON);
