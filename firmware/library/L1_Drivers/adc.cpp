#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/adc.hpp"

LPC_ADC_TypeDef * Adc::adc_base       = LPC_ADC;
LPC_SC_TypeDef * Adc::sysclk_register = LPC_SC;
