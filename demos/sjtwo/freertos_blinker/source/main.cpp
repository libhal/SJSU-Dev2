#include <FreeRTOS.h>
#include <task.h>

#include <cerrno>
#include <cstdint>
#include <cstdio>

#include "config.hpp"
#include "devices/boards/sjtwo.hpp"
#include "devices/switches/button.hpp"
#include "peripherals/lpc40xx/gpio.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/rtos/freertos/rtos.hpp"
#include "utility/time/time.hpp"

#define BOOST_LEAF_EMBEDDED
#define BOOST_LEAF_TLS_FREERTOS
// #define BOOST_LEAF_TLS_ARRAY
#include <boost/leaf.hpp>


// namespace boost { namespace leaf {

// namespace tls
// {
//     inline void * read_void_ptr( int tls_index ) noexcept
//     {
//         printf(".");
//         return pvTaskGetThreadLocalStoragePointer(0, tls_index);
//     }

//     inline void write_void_ptr( int tls_index, void * p ) noexcept
//     {
//         printf("/");
//         vTaskSetThreadLocalStoragePointer(0, tls_index, p);
//     }
// }

// } }

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
  auto combo_number = sjsu::rtos::RetrieveParameter(parameters);

  int press_count = 0;

  if (combo_number == 3)
  {
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
      boost::leaf::try_handle_all(
          [&switch3, &press_count]() -> boost::leaf::result<void>
          {
            if (switch3.Released())
            {
              sjtwo::led3.Toggle();
              return boost::leaf::new_error(press_count++);
            }
            return {};
          },
          [](int press_count_value)
          {
            sjsu::LogInfo("number of presses = %d",
                          press_count_value);
          },
          []() {

          });

      vTaskDelay(50);
    }
  }
  else
  {
    sjsu::Button switch2(sjtwo::button2);
    sjtwo::led2.Initialize();
    switch2.Initialize();

    sjtwo::led2.SetAsOutput();
    sjtwo::led2.SetLow();

    sjsu::LogInfo("SW3 Initialized...");
    sjsu::LogInfo("Press and release SW3 to toggle LED2 state...");

    // Loop detects when the button has been released and changes the LED state
    // accordingly.
    while (true)
    {
      boost::leaf::try_handle_all(
          [&switch2, &press_count]() -> boost::leaf::result<void>
          {
            if (switch2.Released())
            {
              sjtwo::led2.Toggle();
              return boost::leaf::new_error(press_count++);
            }
            return {};
          },
          [](int press_count_value)
          {
            sjsu::LogInfo("number of presses = %d",
                          press_count_value);
          },
          []() {

          });

      vTaskDelay(50);
    }
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
  xTaskCreate(ButtonReader,                 // Make function ButtonReader a task
              "ButtonReaderTask3",          // Task the name "ButtonReaderTask"
              sjsu::rtos::StackSize(1024),  // Size of stack allocated to task
              sjsu::rtos::PassParameter(3),   // Pass nothing to this task
              sjsu::rtos::Priority::kMedium,  // Give this task medium priority
              sjsu::rtos::kNoHandle);         // Do not supply a task handle
  xTaskCreate(ButtonReader,                 // Make function ButtonReader a task
              "ButtonReaderTask2",          // Task the name "ButtonReaderTask"
              sjsu::rtos::StackSize(1024),  // Size of stack allocated to task
              sjsu::rtos::PassParameter(2),   // Pass nothing to this task
              sjsu::rtos::Priority::kMedium,  // Give this task medium priority
              sjsu::rtos::kNoHandle);         // Do not supply a task handle
  sjsu::LogInfo("Starting Scheduler...");
  vTaskStartScheduler();
  return 0;
}

namespace boost
{
void throw_exception(std::exception const & e)
{
  std::abort();
}
}  // namespace boost
