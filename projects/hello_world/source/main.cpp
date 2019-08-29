#include <cstdint>

#include "utility/log.hpp"
#include "utility/time.hpp"
#include "L0_Platform/lpc40xx/LPC40xx.h"

int main(void)
{
    using namespace sjsu::lpc40xx;

    LPC_GPIO1->DIR |= (1 << 24);
    //// Set 0th bit, setting Pin 0.0 to high
    LPC_GPIO1->PIN &= ~(1 << 24);
    //// Loop for a while (volatile is needed!)
    for(volatile uint32_t i = 0; i < 0x0200000; i++);
    //// Clear 0th bit, setting Pin 0.0 to low
    LPC_GPIO1->PIN |= (1 << 24);
    //// Loop forever
    while(1);
    return 0;
}
// int main()
// {
//   LOG_INFO("Staring Hello World Application");
//   while (true)
//   {
//     for (uint8_t i = 0; i < 16; i++)
//     {
//       LOG_INFO("Hello World 0x%X", i);
//       sjsu::Delay(500ms);
//     }
//   }

//   return -1;
// }
