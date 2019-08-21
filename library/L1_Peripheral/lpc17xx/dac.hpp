#pragma once

#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/dac.hpp"

namespace sjsu
{
namespace lpc17xx
{
using ::sjsu::lpc40xx::Dac;

struct DacChannel // NOLINT
{
  static constexpr Pin kDacPin = Pin::CreatePin<0, 26>();
};
}  // namespace lpc17xx
}  // namespace sjsu
