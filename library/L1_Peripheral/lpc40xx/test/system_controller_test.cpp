#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(SystemController);

TEST_CASE("sjsu::lpc40xx::SystemController", "[lpc40xx-system-controller]")
{
  SECTION("Initialize") {}
}
}  // namespace sjsu::lpc40xx
