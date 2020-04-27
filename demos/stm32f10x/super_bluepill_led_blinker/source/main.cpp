#include <cstdio>

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "utility/time.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Starting Super Bluepill LED Blinker Application...");

  ///////////// Setup LED GPIO /////////////
  // Power I/O Port A Peripheral
  sjsu::stm32f10x::RCC->APB2ENR |= (1 << 2);
  // Set as output with 10Mhz clock source
  sjsu::stm32f10x::GPIOA->CRL |= (1 << 4);
  sjsu::stm32f10x::GPIOA->CRL &= ~(1 << 6);
  sjsu::stm32f10x::GPIOA->CRL &= ~(1 << 7);

  int counter = 0;
  while (true)
  {
    // Print hello world over STDOUT. STDOUT is both semihost (gdb debug screen)
    // as well through another means like UART, USB, RS232 and other.
    printf("Hello World %d\n", counter++);

    // Reset Port C Pin 13
    sjsu::stm32f10x::GPIOA->BSRR = (1 << 17);
    sjsu::Delay(100ms);

    // Set Port C Pin 13
    sjsu::stm32f10x::GPIOA->BSRR = (1 << 1);
    sjsu::Delay(100ms);
  }
  return 0;
}
