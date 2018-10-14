#include "L0_LowLevel/interrupt.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing interrupt", "[interrupt]")
{
  auto test_isr = []() {};
  SECTION("RegisterIsr")
  {
    constexpr int32_t kPriority = 5;
    RESET_FAKE(NVIC_EnableIRQ);
    RESET_FAKE(NVIC_SetPriority);
    dynamic_isr_vector_table[WDT_IRQn] = nullptr;

    RegisterIsr(WDT_IRQn, test_isr, true, kPriority);

    CHECK(test_isr == dynamic_isr_vector_table[WDT_IRQn]);
    CHECK(WDT_IRQn == NVIC_EnableIRQ_fake.arg0_val);
    CHECK(WDT_IRQn == NVIC_SetPriority_fake.arg0_val);
    CHECK(kPriority == NVIC_SetPriority_fake.arg1_val);
  }
  SECTION("DeregisterIsr")
  {
    RESET_FAKE(NVIC_DisableIRQ);
    dynamic_isr_vector_table[WDT_IRQn] = test_isr;

    DeregisterIsr(WDT_IRQn);

    CHECK(nullptr == dynamic_isr_vector_table[WDT_IRQn]);
    CHECK(WDT_IRQn == NVIC_DisableIRQ_fake.arg0_val);
  }
}
