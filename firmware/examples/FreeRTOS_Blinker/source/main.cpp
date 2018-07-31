#include <cstdint>
#include <cstdio>

#include "FreeRTOS.h"
#include "task.h"

#include "config.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L2_Utilities/debug_print.hpp"
#include "L2_Utilities/macros.hpp"
#include "L2_Utilities/rtos.hpp"

namespace
{

void LedToggle(void * parameters)
{
    DEBUG_PRINT("Setting up task...");
    DEBUG_PRINT("Retrieving delay amount from parameters...");
    auto delay = rtos::RetrieveParameter(parameters);
    DEBUG_PRINT("Initializing LEDs...");
    LPC_IOCON->P1_1 &= ~(0b111);
    LPC_IOCON->P1_8 &= ~(0b111);
    LPC_GPIO1->DIR |= (1 << 1);
    LPC_GPIO1->PIN &= ~(1 << 1);
    LPC_GPIO1->DIR |= (1 << 8);
    LPC_GPIO1->PIN |= (1 << 8);
    DEBUG_PRINT("LEDs Initialized...");
    DEBUG_PRINT("Toggling LEDs...");
    // Loop blinks the LEDs back and forth at a rate that depends on the
    // pvParameter's value.
    while (true)
    {
        LPC_GPIO1->PIN ^= 0b0001'0000'0010;
        vTaskDelay(delay);
    }
}

constexpr uint32_t kButtonPinNumber = 14;
constexpr uint32_t kLedNumber       = 15;

bool CheckSwitch3()
{
    return (LPC_GPIO1->PIN & (1 << kButtonPinNumber));
}

void ButtonReader(void * parameters)
{
    SJ2_USED(parameters);
    DEBUG_PRINT("Setting up task...");
    DEBUG_PRINT("Initializing LED3 and SW3...");
    LPC_IOCON->P1_14 &= ~(0b111);
    LPC_IOCON->P1_15 &= ~(0b111);
    LPC_GPIO1->DIR &= ~(1 << kButtonPinNumber);
    LPC_GPIO1->DIR |= (1 << kLedNumber);
    LPC_GPIO1->PIN |= (1 << kLedNumber);
    DEBUG_PRINT("LED3 and SW3 Initialized...");
    DEBUG_PRINT("Press and release SW3 to toggle LED3 state...");
    bool button_pressed = false;
    // Loop detects when the button has been released and changes the LED state
    // accordingly.
    while (true)
    {
        if (CheckSwitch3())
        {
            button_pressed = true;
        }
        else if (!CheckSwitch3() && button_pressed)
        {
            LPC_GPIO1->PIN ^= (1 << kLedNumber);
            button_pressed = false;
        }
        else
        {
            button_pressed = false;
        }
        vTaskDelay(50);
    }
}

}  // namespace

int main(void)
{
    TaskHandle_t handle = NULL;
    DEBUG_PRINT("Creating Tasks ...");
    // See https://www.freertos.org/a00125.html for the xTaskCreate API
    // See L2_Utilities/rtos.hpp for the rtos:: namespace utility functions
    xTaskCreate(
        LedToggle,                // Make function LedToggle a task
        "LedToggle",              // Give this task the name "LedToggle"
        rtos::StackSize(256),      // Size of stack allocated to task
        rtos::PassParameter(100),  // Parameter to be passed to task
        rtos::Priority::kLow,      // Give this task low priority
        &handle);                 // Reference to the task
    xTaskCreate(
        ButtonReader,             // Make function ButtonReader a task
        "ButtonReader",           // Give this task the name "ButtonReader"
        rtos::StackSize(256),      // Size of stack allocated to task
        rtos::kNoParameter,        // Pass nothing to this task
        rtos::Priority::kMedium,   // Give this task medium priority
        rtos::kNoHandle);          // Do not supply a task handle
    DEBUG_PRINT("Starting Scheduler ...");
    vTaskStartScheduler();
    return 0;
}
