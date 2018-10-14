#include <inttypes.h>
#include <cstdint>
#include "L0_LowLevel/delay.hpp"
#include "L2_Utilities/debug_print.hpp"

int main(void)
{
  DEBUG_PRINT("Delay Application Starting...");

  DEBUG_PRINT(
      "This example merely prints a statement every second using the delay "
      "function.");

  uint32_t counter = 0;

  while (1)
  {
    DEBUG_PRINT("[%lu] Hello, World! (milliseconds = %lu)", counter++,
                static_cast<uint32_t>(Milliseconds()));
    Delay(1000);
  }
  return 0;
}
