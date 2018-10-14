#include "L2_Utilities/debug_print.hpp"

int main(void)
{
  DEBUG_PRINT("System Timer Application Starting...");

  DEBUG_PRINT(
      "If you look at the source code of this demo, you can see that the "
      "main only prints these messages and then waits in a while loop. Main "
      "isn't doing anything and yet the 3rd LED is blinking?");
  DEBUG_PRINT(
      "If you checkout the low_level_init.cpp file you will see that the "
      "LowLevelInit() function has been overriden and SystemTimer has been "
      "programmed to toggle the LED at a frequency of 10Hz.");

  DEBUG_PRINT("Halting any action.");
  while (1)
  {
    continue;
  }
  return 0;
}
