#include "L1_Peripheral/interrupt.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "third_party/fakeit/fakeit.hpp"

namespace sjsu
{
TEST_CASE("Testing interrupt interface")
{
  using namespace fakeit;  // NOLINT
  SECTION("Should be able to Set and Get PlatformController()")
  {
    // Setup
    Mock<InterruptController> mock_controller;

    // Exercise
    InterruptController::SetPlatformController(&mock_controller.get());

    // Verify
    auto & should_be_the_mock_controller =
        InterruptController::GetPlatformController();
    CHECK(&mock_controller.get() == &should_be_the_mock_controller);
  }
}
}  // namespace sjsu
