#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
// TODO(#632): Add tests back
TEST_CASE("Testing lpc40xx interrupt", "[lpc40xx-interrupt]")
{
  // auto test_isr                 = []() {};
  // constexpr size_t kVectorIndex = WDT_IRQn + lpc40xx::kIrqOffset;
  // SECTION("RegisterIsr")
  // {
  //   constexpr int32_t kPriority = 5;
  //   RESET_FAKE(NVIC_EnableIRQ);
  //   RESET_FAKE(NVIC_SetPriority);
  //   dynamic_isr_vector_table[kVectorIndex] = nullptr;

  //   interrupt_controller_.Register(WDT_IRQn, test_isr, true, kPriority);

  //   CHECK(test_isr == dynamic_isr_vector_table[kVectorIndex]);
  //   CHECK(WDT_IRQn == NVIC_EnableIRQ_fake.arg0_val);
  //   CHECK(WDT_IRQn == NVIC_SetPriority_fake.arg0_val);
  //   CHECK(kPriority == NVIC_SetPriority_fake.arg1_val);
  // }
  // SECTION("DeregisterIsr")
  // {
  //   RESET_FAKE(NVIC_DisableIRQ);
  //   dynamic_isr_vector_table[kVectorIndex] = test_isr;

  //    interrupt_controller_.Deregister(WDT_IRQn);

  //   CHECK(InterruptLookupHandler == dynamic_isr_vector_table[kVectorIndex]);
  //   CHECK(WDT_IRQn == NVIC_DisableIRQ_fake.arg0_val);
  // }
}
}  // namespace sjsu::lpc40xx
