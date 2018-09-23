#pragma once
#include "L0_LowLevel/startup.hpp"

void RegisterIsr(IRQn_Type irq, IsrPointer isr);
