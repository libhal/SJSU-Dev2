#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/startup.hpp"

#if defined HOST_TEST
// NOLINTNEXTLINE(readability-identifier-naming)
IsrPointer dynamic_isr_vector_table[56] = { nullptr };
#endif  // defined HOST_TEST

void RegisterIsr(IRQn_Type irq, IsrPointer isr)
{
    dynamic_isr_vector_table[irq] = isr;
}
