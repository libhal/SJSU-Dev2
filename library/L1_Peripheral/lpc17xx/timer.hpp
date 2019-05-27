#pragma once

#include "L1_Peripheral/lpc40xx/timer.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Timer;
}  // namespace lpc17xx
}  // namespace sjsu
