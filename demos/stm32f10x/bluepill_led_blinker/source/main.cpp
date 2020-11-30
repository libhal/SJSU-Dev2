#include <cstdio>

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "utility/time.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Starting Bluepill LED Blinker Application...");
  ///////////// Setup LED GPIO /////////////

  // Power I/O Port C Peripheral
  sjsu::stm32f10x::RCC->APB2ENR |= (1 << 4);
  // Set as output with 10Mhz clock source
  sjsu::stm32f10x::GPIOC->CRH |= (1 << 20);

  int counter = 0;
  while (true)
  {
    // Print hello world over STDOUT. STDOUT is both semihost (gdb debug screen)
    // as well through another means like UART, USB, RS232 and other.
    printf("Hello World %d\n", counter++);

    // Reset Port C Pin 13
    sjsu::stm32f10x::GPIOC->BSRR = (1 << 29);
    sjsu::Delay(250ms);

    // Set Port C Pin 13
    sjsu::stm32f10x::GPIOC->BSRR = (1 << 13);
    sjsu::Delay(250ms);
  }
  return 0;
}
