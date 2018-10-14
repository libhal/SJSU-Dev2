#include "L0_LowLevel/interrupt.hpp"
#if defined HOST_TEST
// NOLINTNEXTLINE(readability-identifier-naming)
IsrPointer dynamic_isr_vector_table[56] = { nullptr };

DEFINE_FAKE_VOID_FUNC(NVIC_EnableIRQ, IRQn_Type);
DEFINE_FAKE_VOID_FUNC(NVIC_DisableIRQ, IRQn_Type);
DEFINE_FAKE_VOID_FUNC(NVIC_SetPriority, IRQn_Type, uint32_t);
#endif  // defined HOST_TEST

void RegisterIsr(IRQn_Type irq, IsrPointer isr, bool enable_interrupt,
                 int32_t priority)
{
  dynamic_isr_vector_table[irq] = isr;
  if (enable_interrupt)
  {
    NVIC_EnableIRQ(irq);
  }
  if (priority > -1)
  {
    NVIC_SetPriority(irq, priority);
  }
}

void DeregisterIsr(IRQn_Type irq)
{
  NVIC_DisableIRQ(irq);
  dynamic_isr_vector_table[irq] = nullptr;
}
