#include "L1_Peripheral/cortex/interrupt.hpp"

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::cortex
{
TEST_CASE("Testing cortex interrupt")
{
  constexpr uint8_t kIRQ             = 7;
  constexpr uint8_t kPriority        = 2;
  constexpr uint8_t kPriorityBits    = 5;
  constexpr uint8_t kNumberOfVectors = 10;

  InterruptController<kNumberOfVectors, kPriorityBits> test_subject;

  SCB_Type local_scb = {
    // This field must be defined otherwise compiler will complain
    .CPUID = 0,
  };

  testing::ClearStructure(&local_scb);
  test_subject.scb = &local_scb;

  NVIC_Type local_nvic;
  testing::ClearStructure(&local_nvic);
  test_subject.nvic = &local_nvic;

  // Making the unregistered handler nullptr guarantees that an exception will
  // be thrown if an exception occurs, which is useful in the "Initialize" test
  // section.
  test_subject.Initialize(nullptr);

  SECTION("Initialize")
  {
    for (uint32_t irq = 0; irq < kNumberOfVectors; irq++)
    {
      // Setup
      // Set the active interrupt to
      local_scb.ICSR = irq;

      // Exercise & Verify
      CHECK_THROWS(test_subject.LookupHandler());
    }
  }

  SECTION("Enable")
  {
    // Setup
    bool callback_was_registered_and_called = false;

    // Exercise
    test_subject.Enable({
        .interrupt_request_number = kIRQ,
        .interrupt_handler =
            [&callback_was_registered_and_called]() {
              callback_was_registered_and_called = true;
            },
        .priority = kPriority,
    });
    // Set the active interrupt to kIRQ.
    local_scb.ICSR = test_subject.IRQToIndex(kIRQ);
    // Should call the service routine above.
    test_subject.LookupHandler();

    // Verify
    CHECK(callback_was_registered_and_called);
    CHECK(kIRQ == test_subject.current_vector);
    CHECK((kPriority << (8 - kPriorityBits)) == local_nvic.IP[kIRQ]);
    CHECK(local_nvic.ISER[(kIRQ >> 5)] == (1 << (kIRQ & 0x1F)));
  }
  SECTION("Disable")
  {
    test_subject.Disable(kIRQ);
    CHECK(local_nvic.ICER[(kIRQ >> 5)] == (1 << (kIRQ & 0x1F)));
  }
}
}  // namespace sjsu::cortex
