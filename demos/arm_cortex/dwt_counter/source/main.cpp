#include "L1_Peripheral/cortex/dwt_counter.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Data Watchdog & Trace Counter Application Starting...");
  sjsu::cortex::DwtCounter dwt_counter;
  dwt_counter.Initialize();

  // We are storing the counts in an array to reduce the latency between each
  // call to GetCount(). If we added a call to printf/LogInfo, this will
  // introduce latency into the counts, making the tick counts seem larger then
  // they really are.
  constexpr size_t kCountTotal = 25;
  uint32_t counts[kCountTotal];

  for (size_t i = 0; i < kCountTotal; i++)
  {
    // NOTE: The counter increments on clock rising and falling. To get back our
    // system clock rate, the value must be divided by 2.
    counts[i] = dwt_counter.GetCount() / 2;
    sjsu::Delay(10ms);
  }

  for (size_t i = 0; i < kCountTotal; i++)
  {
    sjsu::LogInfo("Cycle count = %lu @ %zu\n", counts[i], i);
  }

  return 0;
}
