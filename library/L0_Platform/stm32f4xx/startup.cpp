#include "L0_Platform/startup.hpp"

#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>
#include <cstring>
#include <iterator>

#include "L0_Platform/ram.hpp"
#include "L0_Platform/stm32f4xx/stm32f4xx.h"
#include "L1_Peripheral/cortex/dwt_counter.hpp"
#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/stm32f4xx/system_controller.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"

// Private namespace to make sure that these do not conflict with other globals
namespace
{
using sjsu::cortex::InterruptController;
// Create stm32f4xx system controller to be used by low level initialization.
sjsu::stm32f4xx::SystemController system_controller;
// Create timer0 to be used by lower level initialization for uptime calculation
sjsu::cortex::DwtCounter arm_dwt_counter;
// System timer is used to count milliseconds of time and to run the RTOS
// scheduler.
sjsu::cortex::SystemTimer system_timer(
    sjsu::stm32f4xx::SystemController::Peripherals::kSystemTimer,
    configKERNEL_INTERRUPT_PRIORITY);
// Cortex NVIC interrupt controller used to setup interrupt service routines
sjsu::cortex::InterruptController<sjsu::lpc40xx::kNumberOfIrqs,
                                  __NVIC_PRIO_BITS>
    interrupt_controller;
}  // namespace

extern "C" uint32_t ThreadRuntimeCounter()
{
  return arm_dwt_counter.GetCount();
}

// The entry point for the C++ library startup
extern "C"
{
  // These are defined after the compilation of the FreeRTOS port for Cortex M4F
  // These will link to those definitions.
  extern void xPortPendSVHandler(void);   // NOLINT
  extern void vPortSVCHandler(void);      // NOLINT
  extern void xPortSysTickHandler(void);  // NOLINT
  // Forward declaration of the default handlers. These are aliased.
  // When the application defines a handler (with the same name), the
  // application's handler will automatically take precedence over these weak
  // definitions.
  extern void ArmHardFaultHandler(void);
  extern void ArmResetHandler(void);

  void vPortSetupTimerInterrupt(void)  // NOLINT
  {
    // Disable timer so the callback can be configured
    system_timer.Enable(false);

    // Set the SystemTick frequency to the RTOS tick frequency
    // It is critical that this happens before you set the system_clock,
    // since The system_timer keeps the time that the system_clock uses to
    // delay itself.
    system_timer.ConfigureCallback(xPortSysTickHandler);

    // Re-enable timer
    system_timer.Enable(true);
  }
}

// The Interrupt vector table.
// This relies on the linker script to place at correct location in memory.
SJ2_SECTION(".isr_vector")
// NOLINTNEXTLINE(readability-identifier-naming)
const sjsu::InterruptVectorAddress kInterruptVectorTable[] = {
  // Core Level - CM4
  &StackTop,                           // 0, The initial stack pointer
  ArmResetHandler,                     // 1, The reset handler
  interrupt_controller.LookupHandler,  // 2, The NMI handler
  ArmHardFaultHandler,                 // 3, The hard fault handler
  interrupt_controller.LookupHandler,  // 4, The MPU fault handler
  interrupt_controller.LookupHandler,  // 5, The bus fault handler
  interrupt_controller.LookupHandler,  // 6, The usage fault handler
  nullptr,                             // 7, Reserved
  nullptr,                             // 8, Reserved
  nullptr,                             // 9, Reserved
  nullptr,                             // 10, Reserved
  vPortSVCHandler,                     // 11, SVCall handler
  interrupt_controller.LookupHandler,  // 12, Debug monitor handler
  nullptr,                             // 13, Reserved
  xPortPendSVHandler,                  // 14, FreeRTOS PendSV Handler
  interrupt_controller.LookupHandler,  // 15, The SysTick handler
  // Chip Level - stm32f4xx
  interrupt_controller.LookupHandler,  // 0, Window WatchDog
  interrupt_controller.LookupHandler,  // 1, PVD through EXTI Line detection
  interrupt_controller.LookupHandler,  // 2, Tamper and TimeStamp interrupts
                                       // through the EXTI line
  interrupt_controller.LookupHandler,  // 3, RTC Wakeup interrupt through the
                                       //    EXTI line
  interrupt_controller.LookupHandler,  // 4, FLASH
  interrupt_controller.LookupHandler,  // 5, RCC
  interrupt_controller.LookupHandler,  // 6, EXTI Line0
  interrupt_controller.LookupHandler,  // 7, EXTI Line1
  interrupt_controller.LookupHandler,  // 8, EXTI Line2
  interrupt_controller.LookupHandler,  // 9, EXTI Line3
  interrupt_controller.LookupHandler,  // 10, EXTI Line4
  interrupt_controller.LookupHandler,  // 11, DMA1 Stream 0
  interrupt_controller.LookupHandler,  // 12, DMA1 Stream 1
  interrupt_controller.LookupHandler,  // 13, DMA1 Stream 2
  interrupt_controller.LookupHandler,  // 14, DMA1 Stream 3
  interrupt_controller.LookupHandler,  // 15, DMA1 Stream 4
  interrupt_controller.LookupHandler,  // 16, DMA1 Stream 5
  interrupt_controller.LookupHandler,  // 17, DMA1 Stream 6
  interrupt_controller.LookupHandler,  // 18, ADC1, ADC2 and ADC3s
  interrupt_controller.LookupHandler,  // 19, CAN1 TX
  interrupt_controller.LookupHandler,  // 20, CAN1 RX0
  interrupt_controller.LookupHandler,  // 21, CAN1 RX1
  interrupt_controller.LookupHandler,  // 22, CAN1 SCE
  interrupt_controller.LookupHandler,  // 23, External Line[9:5]s
  interrupt_controller.LookupHandler,  // 24, TIM1 Break interrupt and TIM9
  interrupt_controller.LookupHandler,  // 25, TIM1 Update and TIM10
  interrupt_controller.LookupHandler,  // 26, TIM1 Trigger and Commutation and
                                       //     TIM11
  interrupt_controller.LookupHandler,  // 27, TIM1 Capture Compare
  interrupt_controller.LookupHandler,  // 28, TIM2
  interrupt_controller.LookupHandler,  // 29, TIM3
  interrupt_controller.LookupHandler,  // 30, TIM4
  interrupt_controller.LookupHandler,  // 31, I2C1 Event
  interrupt_controller.LookupHandler,  // 32, I2C1 Error
  interrupt_controller.LookupHandler,  // 33, I2C2 Event
  interrupt_controller.LookupHandler,  // 34, I2C2 Error
  interrupt_controller.LookupHandler,  // 35, SPI1
  interrupt_controller.LookupHandler,  // 36, SPI2
  interrupt_controller.LookupHandler,  // 37, USART1
  interrupt_controller.LookupHandler,  // 38, USART2
  interrupt_controller.LookupHandler,  // 39, USART3
  interrupt_controller.LookupHandler,  // 40, External Line[15:10]s
  interrupt_controller.LookupHandler,  // 41, RTC Alarm (A and B) through EXTI
                                       //     Line
  interrupt_controller.LookupHandler,  // 42, USB OTG FS Wakeup through EXTI
                                       //     line interrupt
  interrupt_controller.LookupHandler,  // 43, TIM8 Break and TIM12  =
  interrupt_controller.LookupHandler,  // 44, TIM8 Update and TIM13
  interrupt_controller.LookupHandler,  // 45, TIM8 Trigger and Commutation and
                                       //     TIM14
  interrupt_controller.LookupHandler,  // 46, TIM8 Capture Compare
  interrupt_controller.LookupHandler,  // 47, DMA1 Stream7
  interrupt_controller.LookupHandler,  // 48, FSMC
  interrupt_controller.LookupHandler,  // 49, SDIO
  interrupt_controller.LookupHandler,  // 50, TIM5
  interrupt_controller.LookupHandler,  // 51, SPI3
  interrupt_controller.LookupHandler,  // 52, UART4
  interrupt_controller.LookupHandler,  // 53, UART5
  interrupt_controller.LookupHandler,  // 54, TIM6 global and DAC1&2 underrun
                                       //     error
  interrupt_controller.LookupHandler,  // 55, TIM7
  interrupt_controller.LookupHandler,  // 56, DMA2 Stream 0
  interrupt_controller.LookupHandler,  // 57, DMA2 Stream 1
  interrupt_controller.LookupHandler,  // 58, DMA2 Stream 2
  interrupt_controller.LookupHandler,  // 59, DMA2 Stream 3
  interrupt_controller.LookupHandler,  // 60, DMA2 Stream 4
  interrupt_controller.LookupHandler,  // 61, Ethernet
  interrupt_controller.LookupHandler,  // 62, Ethernet Wakeup through EXTI line
  interrupt_controller.LookupHandler,  // 63, CAN2 TX
  interrupt_controller.LookupHandler,  // 64, CAN2 RX0
  interrupt_controller.LookupHandler,  // 65, CAN2 RX1
  interrupt_controller.LookupHandler,  // 66, CAN2 SCE
  interrupt_controller.LookupHandler,  // 67, USB OTG FS
  interrupt_controller.LookupHandler,  // 68, DMA2 Stream 5
  interrupt_controller.LookupHandler,  // 69, DMA2 Stream 6
  interrupt_controller.LookupHandler,  // 70, DMA2 Stream 7
  interrupt_controller.LookupHandler,  // 71, USART6
  interrupt_controller.LookupHandler,  // 72, I2C3 event interrupt
  interrupt_controller.LookupHandler,  // 73, I2C3 error interrupt
  interrupt_controller.LookupHandler,  // 74, USB OTG HS End Point 1 Out
  interrupt_controller.LookupHandler,  // 75, USB OTG HS End Point 1 In
  interrupt_controller.LookupHandler,  // 76, USB OTG HS Wakeup through EXTI
  interrupt_controller.LookupHandler,  // 77, USB OTG HS
  interrupt_controller.LookupHandler,  // 78, DCMI
  interrupt_controller.LookupHandler,  // 79, CRYP crypto
  interrupt_controller.LookupHandler,  // 80, Hash and Rng
  interrupt_controller.LookupHandler,  // 81, FPU
};

namespace sjsu
{
SJ2_WEAK(void InitializePlatform());
void InitializePlatform()
{
  // NOTE: Use default STDOUT and STDIN which do nothing.
  // sjsu::newlib::SetStdout(...);
  // sjsu::newlib::SetStdin(...);

  // Set the platform's interrupt controller.
  // This will be used by other libraries to enable and disable interrupts.
  sjsu::InterruptController::SetPlatformController(&interrupt_controller);
  sjsu::SystemController::SetPlatformController(&system_controller);

  system_timer.Initialize();
  system_timer.ConfigureTickFrequency(config::kRtosFrequency);
  system_timer.ConfigureCallback([]() {});
  system_timer.Enable();

  arm_dwt_counter.Initialize();
  sjsu::SetUptimeFunction(sjsu::cortex::SystemTimer::GetCount);
}
}  // namespace sjsu
