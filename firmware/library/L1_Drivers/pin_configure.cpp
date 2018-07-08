#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin_configure.hpp"

PinConfigure::PinMap_t * PinConfigure::pin_map =
    reinterpret_cast<PinMap_t *>(LPC_IOCON);
