#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/msp432p401r/system_controller.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/units.hpp"

namespace sjsu
{
namespace msp432p401r
{
EMIT_ALL_METHODS(SystemController);

TEST_CASE("Testing msp432p401r SystemController",
          "[msp432p401r-system-controller]")
{
  SystemController system_controller;

  SECTION("SetSystemClockFrequency") {}
  SECTION("SetPeripheralClockDivider") {}
  SECTION("GetPeripheralClockDivider") {}
  SECTION("GetSystemFrequency")
  {
    constexpr units::frequency::hertz_t kExpectedCpuSpeed = 3_MHz;
    CHECK(system_controller.GetSystemFrequency() <= kExpectedCpuSpeed);
  }
  SECTION("IsPeripheralPoweredUp") {}
  SECTION("PowerUpPeripheral") {}
  SECTION("PowerDownPeripheral") {}
}
}  // namespace msp432p401r
}  // namespace sjsu
