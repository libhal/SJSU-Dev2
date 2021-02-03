#include <cstdio>

#include "platforms/targets/stm32f4xx/stm32f4xx.h"
#include "utility/time/time.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Starting FK407M1 LED Blinker Application...");
  ///////////// Setup LED GPIO /////////////

  // Power I/O Port C Peripheral
  sjsu::stm32f4xx::RCC->AHB1ENR |= (1 << 2);
  // Set as output
  sjsu::stm32f4xx::GPIOC->MODER |= (1 << 26);

  int counter = 0;
  while (true)
  {
    // Print hello world over STDOUT. STDOUT is both semihost (gdb debug screen)
    // as well through another means like UART, USB, RS232 and other.
    printf("Hello World %d\n", counter++);

    // Reset Port C Pin 13
    sjsu::stm32f4xx::GPIOC->BSRRH = (1 << 13);
    sjsu::Delay(250ms);

    // Set Port C Pin 13
    sjsu::stm32f4xx::GPIOC->BSRRL = (1 << 13);
    sjsu::Delay(250ms);
  }
  return 0;
}
