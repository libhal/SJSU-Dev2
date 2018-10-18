#include "L1_Drivers/dac.hpp"

LPC_DAC_TypeDef * Dac::dac_register     = LPC_DAC;
LPC_SC_TypeDef * Dac::sc_ptr            = LPC_SC;
LPC_IOCON_TypeDef * Dac::iocon_register = LPC_IOCON;
