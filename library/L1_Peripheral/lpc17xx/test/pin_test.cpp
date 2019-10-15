// Test for Pin class.
// Using a test by side effect on the LPC_IOCON register
#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/debug.hpp"

namespace sjsu::lpc17xx
{
EMIT_ALL_METHODS(Pin);

TEST_CASE("Testing lpc17xx Pin", "[lpc17xx-pin]") {}
}  // namespace sjsu::lpc17xx
