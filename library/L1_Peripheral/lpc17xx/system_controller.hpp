#pragma once

#include "L1_Peripheral/lpc40xx/system_controller.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::SystemController;
}  // namespace lpc17xx
}  // namespace sjsu
