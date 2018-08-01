#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/gpio.hpp"

// GPIO ports defined within gpio_base
LPC_GPIO_TypeDef * Gpio::gpio_base[6] =
    {LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4, LPC_GPIO5};
