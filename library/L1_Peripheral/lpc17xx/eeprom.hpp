#pragma once

#include "L1_Peripheral/lpc40xx/eeprom.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40XX driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Eeprom;
}  // namespace lpc17xx
}  // namespace sjsu
