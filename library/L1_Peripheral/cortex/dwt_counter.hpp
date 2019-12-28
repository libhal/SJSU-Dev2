#pragma once

#include "L0_Platform/lpc40xx/LPC40xx.h"

namespace sjsu
{
namespace cortex
{
/// The DWT (Debug Watch and Trace) module in the Cortex M series of processors
/// has a number of debugging
class DwtCounter
{
 public:
  /// Address of the hardware DWT registers
  static inline DWT_Type * dwt = DWT;
  /// Address of the Cortex M CoreDebug module
  static inline CoreDebug_Type * core = CoreDebug;

  /// Initialize the debug core to enable counting and then being counting on
  /// the DWT.
  void Initialize()
  {
    core->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    dwt->CYCCNT = 0;
    dwt->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }

  /// Return the current number of ticks. Note that this is typically 2x the
  /// system frequency as it counts on rising and falling edges.
  uint32_t GetCount()
  {
    return dwt->CYCCNT;
  }
};
}  // namespace cortex
}  // namespace sjsu
