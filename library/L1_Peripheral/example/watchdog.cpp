#include <cstdint>
#include <iterator>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/watchdog.hpp"
#include "L3_Application/task.hpp"
#include "utility/time.hpp"
#include "utility/rtos.hpp"
#include "utility/log.hpp"

uint32_t timer_value;
uint16_t timer_warning;

sjsu::lpc40xx::Watchdog watchdog;

uint8_t button3_port = 0;
uint8_t button3_pin = 29;
sjsu::lpc40xx::Gpio button3(button3_port, button3_pin);

uint8_t led3_port = 1;
uint8_t led3_pin = 18;
sjsu::lpc40xx::Gpio led3(led3_port, led3_pin);

void FeedTask(void * pv_paramater)
{
  while (true)
  {
    watchdog.FeedSequence();
  }
}

void HighCPUTask(void * pv_paramater)
{
  while (true)
  {
     printf("High Task! \n");
     sjsu::Delay(500ms);
    if (button3.Read()) {
      led3.SetLow();
    }
    else {
      led3.SetHigh();
    }
  }
}

int main()
{
  button3.SetAsInput();
  watchdog.Initialize(0x02'FFFF, 0xFF'FFFF, 0b11'1111'1111);
  watchdog.RegisterIrqHandler();
  watchdog.FeedSequence();

  xTaskCreate(FeedTask, "FeedTask", 1024, NULL, sjsu::rtos::kLow, NULL);
  xTaskCreate(HighCPUTask, "HighCPUTask", 1024, NULL, sjsu::rtos::kHigh, NULL);
  vTaskStartScheduler();

  sjsu::Halt();
}
