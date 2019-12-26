#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/lpc40xx/timer.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Timer);
TEST_CASE("Testing lpc40xx Timer", "[lpc40xx-timer]") {}
}  // namespace sjsu::lpc40xx
