#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>
#include <cstdio>

#include "peripherals/lpc40xx/gpio.hpp"
#include "devices/boards/sjtwo.hpp"
#include "devices/switches/button.hpp"
#include "config.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/rtos/freertos/rtos.hpp"
#include "utility/time/time.hpp"

// Using anonymous namespace so these tasks are only visible to this file
namespace
{
void LedToggle(void * parameters)
{
  sjsu::LogInfo("Setting up task...");
  sjsu::LogInfo("Retrieving delay amount from parameters...");
  auto delay = sjsu::rtos::RetrieveParameter(parameters);

  sjsu::LogInfo("Initializing LEDs...");

  sjtwo::led0.Initialize();
  sjtwo::led1.Initialize();

  sjtwo::led0.SetAsOutput();
  sjtwo::led1.SetAsOutput();

  sjtwo::led0.SetHigh();
  sjtwo::led1.SetLow();

  sjsu::LogInfo("LEDs Initialized...");

  // Loop blinks the LEDs back and forth at a rate that depends on the
  // pvParameter's value.
  sjsu::LogInfo("Toggling LEDs...");

  while (true)
  {
    sjtwo::led0.Toggle();
    sjtwo::led1.Toggle();

    vTaskDelay(delay);
  }
}

void ButtonReader([[maybe_unused]] void * parameters)
{
  sjsu::LogInfo("Setting up task...");
  sjsu::LogInfo("Initializing SW3...");

  sjsu::lpc40xx::Gpio & button_gpio3 = sjsu::lpc40xx::GetGpio<0, 29>();
  sjsu::Button switch3(button_gpio3);

  sjtwo::led3.Initialize();
  switch3.Initialize();

  sjtwo::led3.SetAsOutput();
  sjtwo::led3.SetLow();

  sjsu::LogInfo("SW3 Initialized...");
  sjsu::LogInfo("Press and release SW3 to toggle LED3 state...");

  // Loop detects when the button has been released and changes the LED state
  // accordingly.
  while (true)
  {
    if (switch3.Released())
    {
      sjtwo::led3.Toggle();
    }

    vTaskDelay(50);
  }
}
}  // namespace

TaskHandle_t handle = NULL;

int main()
{
  sjsu::LogInfo("Starting FreeRTOS Blinker Example...");
  sjsu::LogInfo("Creating Tasks...");
  sjsu::Delay(1s);
  // See https://www.freertos.org/a00125.html for the xTaskCreate API
  // See utility/rtos.hpp for the sjsu::rtos:: namespace utility functions
  xTaskCreate(LedToggle,        // Make function LedToggle a task
              "LedToggleTask",  // Give this task the name "LedToggleTask"
              sjsu::rtos::StackSize(1024),  // Size of stack allocated to task
              sjsu::rtos::PassParameter(100),  // Parameter to be passed to task
              sjsu::rtos::Priority::kLow,      // Give this task low priority
              &handle);                        // Optional reference to the task
  xTaskCreate(ButtonReader,        // Make function ButtonReader a task
              "ButtonReaderTask",  // Give this task the name "ButtonReaderTask"
              sjsu::rtos::StackSize(1024),    // Size of stack allocated to task
              sjsu::rtos::kNoParameter,       // Pass nothing to this task
              sjsu::rtos::Priority::kMedium,  // Give this task medium priority
              sjsu::rtos::kNoHandle);         // Do not supply a task handle
  sjsu::LogInfo("Starting Scheduler...");
  vTaskStartScheduler();
  return 0;
}
