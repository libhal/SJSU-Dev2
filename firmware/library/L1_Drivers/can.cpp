#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/can.hpp"

LPC_CAN_TypeDef * Can::can_registers[2] = {LPC_CAN1, LPC_CAN2};
LPC_CANCR_TypeDef * Can::can_central_register = LPC_CANCR;
LPC_CANAF_TypeDef * Can::can_acceptance_filter_register = LPC_CANAF;
LPC_SC_TypeDef * Can::system_control_register = LPC_SC;
