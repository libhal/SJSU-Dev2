#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>
#include <cstring>
#include <iterator>

#include "L0_Platform/ram.hpp"
#include "L0_Platform/startup.hpp"
#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/cortex/dwt_counter.hpp"
#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/msp432p401r/system_controller.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"

// Private namespace to make sure that these do not conflict with other globals
namespace
{
using sjsu::cortex::InterruptController;

/// Clock configuration object for MSP432P401R devices.
sjsu::msp432p401r::SystemController::ClockConfiguration_t clock_configuration;
/// Create msp432p401r system controller to be used by low level initialization.
sjsu::msp432p401r::SystemController system_controller(clock_configuration);

/// Create timer0 to be used by lower level initialization for uptime
/// calculation
sjsu::cortex::DwtCounter arm_dwt_counter;

/// System timer is used to count milliseconds of time and to run the RTOS
/// scheduler.
sjsu::cortex::SystemTimer system_timer(
    sjsu::msp432p401r::SystemController::Modules::kMasterClock,
    configKERNEL_INTERRUPT_PRIORITY);
/// Cortex NVIC interrupt controller used to setup interrupt service routines
sjsu::cortex::InterruptController<sjsu::msp432p401r::kNumberOfIrqs,
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
  interrupt_controller.LookupHandler,  // 11, SVCall handler
  interrupt_controller.LookupHandler,  // 12, Debug monitor handler
  nullptr,                             // 13, Reserved
  interrupt_controller.LookupHandler,  // 14, FreeRTOS PendSV Handler
  interrupt_controller.LookupHandler,  // 15, The SysTick handler
  // Chip Level - msp432p401r
  interrupt_controller.LookupHandler,  // 16 PSS Interrupt
  interrupt_controller.LookupHandler,  // 17 CS Interrupt
  interrupt_controller.LookupHandler,  // 18 PCM Interrupt
  interrupt_controller.LookupHandler,  // 19 WDT_A Interrupt
  interrupt_controller.LookupHandler,  // 20 FPU Interrupt
  interrupt_controller.LookupHandler,  // 21 FLCTL Interrupt
  interrupt_controller.LookupHandler,  // 22 COMP_E0 Interrupt
  interrupt_controller.LookupHandler,  // 23 COMP_E1 Interrupt
  interrupt_controller.LookupHandler,  // 24 TA0_0 Interrupt
  interrupt_controller.LookupHandler,  // 25 TA0_N Interrupt
  interrupt_controller.LookupHandler,  // 26 TA1_0 Interrupt
  interrupt_controller.LookupHandler,  // 27 TA1_N Interrupt
  interrupt_controller.LookupHandler,  // 28 TA2_0 Interrupt
  interrupt_controller.LookupHandler,  // 29 TA2_N Interrupt
  interrupt_controller.LookupHandler,  // 30 TA3_0 Interrupt
  interrupt_controller.LookupHandler,  // 31 TA3_N Interrupt
  interrupt_controller.LookupHandler,  // 32 EUSCIA0 Interrupt
  interrupt_controller.LookupHandler,  // 33 EUSCIA1 Interrupt
  interrupt_controller.LookupHandler,  // 34 EUSCIA2 Interrupt
  interrupt_controller.LookupHandler,  // 35 EUSCIA3 Interrupt
  interrupt_controller.LookupHandler,  // 36 EUSCIB0 Interrupt
  interrupt_controller.LookupHandler,  // 37 EUSCIB1 Interrupt
  interrupt_controller.LookupHandler,  // 38 EUSCIB2 Interrupt
  interrupt_controller.LookupHandler,  // 39 EUSCIB3 Interrupt
  interrupt_controller.LookupHandler,  // 40 ADC14 Interrupt
  interrupt_controller.LookupHandler,  // 41 T32_INT1 Interrupt
  interrupt_controller.LookupHandler,  // 42 T32_INT2 Interrupt
  interrupt_controller.LookupHandler,  // 43 T32_INTC Interrupt
  interrupt_controller.LookupHandler,  // 44 AES256 Interrupt
  interrupt_controller.LookupHandler,  // 45 RTC_C Interrupt
  interrupt_controller.LookupHandler,  // 46 DMA_ERR Interrupt
  interrupt_controller.LookupHandler,  // 47 DMA_INT3 Interrupt
  interrupt_controller.LookupHandler,  // 48 DMA_INT2 Interrupt
  interrupt_controller.LookupHandler,  // 49 DMA_INT1 Interrupt
  interrupt_controller.LookupHandler,  // 50 DMA_INT0 Interrupt
  interrupt_controller.LookupHandler,  // 51 PORT1 Interrupt
  interrupt_controller.LookupHandler,  // 52 PORT2 Interrupt
  interrupt_controller.LookupHandler,  // 53 PORT3 Interrupt
  interrupt_controller.LookupHandler,  // 54 PORT4 Interrupt
  interrupt_controller.LookupHandler,  // 55 PORT5 Interrupt
  interrupt_controller.LookupHandler,  // 56 PORT6 Interrupt
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

  system_controller.Initialize();

  system_timer.Initialize();
  system_timer.ConfigureTickFrequency(config::kRtosFrequency);
  system_timer.ConfigureCallback([]() {});
  system_timer.Enable();

  arm_dwt_counter.Initialize();
  sjsu::SetUptimeFunction(sjsu::cortex::SystemTimer::GetCount);
}
}  // namespace sjsu
