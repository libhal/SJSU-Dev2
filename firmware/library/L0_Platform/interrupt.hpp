#pragma once

#include <cstdint>

// Define an alias for the Isr function pointer.
using IsrPointer = void (*)(void);

namespace sjsu
{
void RegisterIsr(int32_t irq, IsrPointer isr, bool enable_interrupt = true,
                 int32_t priority = -1);
void DeregisterIsr(int32_t irq);
}  // namespace sjsu
