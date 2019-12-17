#include <stdint.h>
#include <stdio.h>
#include "utility/time.hpp"
#include "L0_Platform/stm32f10x/stm32f10x.h"

int main()
{
  ///////////// Setup GPIO /////////////

  // Setup LED GPIO
  // Power I/O PortC Peripheral
  RCC->APB2ENR |= (1 << 4);
  // Set as output with 10Mhz
  GPIOC->CRH |= (1 << 20);
  GPIOC->BSRR = (1 << 29);

  // Reset Port C Pin 13
  GPIOC->BSRR = (1 << 29);

  sjsu::Delay(1s);

  // Uart1Setup();

  // Set Port C Pin 13
  GPIOC->BSRR = (1 << 13);
  sjsu::Delay(1s);

  while (1)
  {
    // Reset Port C Pin 13
    GPIOC->BSRR = (1 << 29);

    printf("Hello World");

    sjsu::Delay(250ms);

    // Set Port C Pin 13
    GPIOC->BSRR = (1 << 13);

    sjsu::Delay(250ms);
  }
  return 0;
}
