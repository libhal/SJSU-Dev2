#include <cstdint>
#include <cstdio>

#include "FreeRTOS.h"
#include "task.h"

#include "config.hpp"
#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L2_Utilities/debug_print.hpp"
#include "L2_Utilities/macros.hpp"
#include "L2_Utilities/rtos.hpp"
#include "L3_HAL/onboard_led.hpp"

// Using anonymous namespace so these tasks are only visible to this file
namespace
{
OnBoardLed leds;

void LedToggle(void * parameters)
{
  DEBUG_PRINT("Setting up task...");
  DEBUG_PRINT("Retrieving delay amount from parameters...");
  auto delay = rtos::RetrieveParameter(parameters);
  DEBUG_PRINT("Initializing LEDs...");
  leds.Off(0);
  leds.On(1);
  DEBUG_PRINT("LEDs Initialized...");
  // Loop blinks the LEDs back and forth at a rate that depends on the
  // pvParameter's value.
  DEBUG_PRINT("Toggling LEDs...");
  while (true)
  {
    leds.Toggle(0);
    leds.Toggle(1);
    vTaskDelay(delay);
  }
}

void ButtonReader(void * parameters)
{
  SJ2_USED(parameters);
  DEBUG_PRINT("Setting up task...");
  DEBUG_PRINT("Initializing SW3...");
  Gpio switch3(1, 14);
  switch3.SetAsInput();
  leds.Off(3);
  DEBUG_PRINT("SW3 Initialized...");
  DEBUG_PRINT("Press and release SW3 to toggle LED3 state...");
  bool previously_pressed = false;
  // Loop detects when the button has been released and changes the LED state
  // accordingly.
  while (true)
  {
    bool pressed = (switch3.Read() == GpioInterface::State::kHigh);
    if (pressed)
    {
      previously_pressed = true;
    }
    else if (!pressed && previously_pressed)
    {
      leds.Toggle(3);
      previously_pressed = false;
    }
    else
    {
      previously_pressed = false;
    }
    vTaskDelay(50);
  }
}

}  // namespace

int main(void)
{
  TaskHandle_t handle = NULL;
  DEBUG_PRINT("Starting FreeRTOS Blinker Example...");
  DEBUG_PRINT("Initializing LEDs...");
  leds.Initialize();
  DEBUG_PRINT("Creating Tasks...");
  // See https://www.freertos.org/a00125.html for the xTaskCreate API
  // See L2_Utilities/rtos.hpp for the rtos:: namespace utility functions
  xTaskCreate(LedToggle,                 // Make function LedToggle a task
              "LedToggle",               // Give this task the name "LedToggle"
              rtos::StackSize(1024),     // Size of stack allocated to task
              rtos::PassParameter(100),  // Parameter to be passed to task
              rtos::Priority::kLow,      // Give this task low priority
              &handle);                  // Optional feference to the task
  xTaskCreate(ButtonReader,              // Make function ButtonReader a task
              "ButtonReader",         // Give this task the name "ButtonReader"
              rtos::StackSize(1024),  // Size of stack allocated to task
              rtos::kNoParameter,     // Pass nothing to this task
              rtos::Priority::kMedium,  // Give this task medium priority
              rtos::kNoHandle);         // Do not supply a task handle
  DEBUG_PRINT("Starting Scheduler ...");
  vTaskStartScheduler();
  return 0;
}
