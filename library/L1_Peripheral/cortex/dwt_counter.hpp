#pragma once

#include "L0_Platform/lpc40xx/LPC40xx.h"

// using ::sjsu::lpc40xx::DWT_Type;
// using ::sjsu::lpc40xx::CoreDebug_Type;

namespace sjsu
{
namespace cortex
{
class DwtCounter
{
 public:
  static inline DWT_Type * dwt = DWT;
  static inline CoreDebug_Type * core = CoreDebug;

  void Initialize()
  {
    core->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    dwt->CYCCNT  = 0;
    dwt->CTRL   |= DWT_CTRL_CYCCNTENA_Msk;
  }
  uint32_t GetCount()
  {
    return dwt->CYCCNT;
  }
};
}  // namespace cortex
}  // namespace sjsu
