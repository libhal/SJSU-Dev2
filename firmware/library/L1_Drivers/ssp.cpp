#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/ssp.hpp"

LPC_SSP_TypeDef * Ssp::ssp_registers[2] = { LPC_SSP0, LPC_SSP1 };
LPC_SC_TypeDef * Ssp::sysclock_register = LPC_SC;
