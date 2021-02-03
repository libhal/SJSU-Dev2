#pragma once

#include "peripherals/lpc40xx/can.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Can;
using ::sjsu::lpc40xx::GetCan;
}  // namespace lpc17xx
}  // namespace sjsu
