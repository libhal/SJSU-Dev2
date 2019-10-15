#include "L0_Platform/lpc40xx/LPC40xx.h"
// TODO(#357): Fake queue.h functions in rtos.hpp
// #include "L1_Peripheral/lpc40xx/can.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
// Uncomment this when can class has been created
// EMIT_ALL_METHODS(Can);

TEST_CASE("Testing lpc40xx Can", "[lpc40xx-can]")
{
  SECTION("Initialize") {}
}
}  // namespace sjsu::lpc40xx
