#pragma once
#include "L0_LowLevel/LPC40xx.h"

// Define an alias for the Isr function pointer.
using IsrPointer = void (*)(void);

void RegisterIsr(IRQn_Type irq, IsrPointer isr);
