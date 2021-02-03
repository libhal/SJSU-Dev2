#include "platforms/utility/startup.hpp"

#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>
#include <cstring>
#include <iterator>

#include "platforms/utility/ram.hpp"
#include "peripherals/cortex/dwt_counter.hpp"
#include "peripherals/cortex/system_timer.hpp"
#include "peripherals/interrupt.hpp"
#include "peripherals/lpc17xx/system_controller.hpp"
#include "peripherals/lpc17xx/uart.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/time/time.hpp"

// Private namespace to make sure that these do not conflict with other globals
namespace
{
/// Clock configuration object for LPC17xx devices.
sjsu::lpc17xx::SystemController::ClockConfiguration_t clock_configuration;

/// Create LPC17xx system controller to be used by low level initialization.
sjsu::lpc17xx::SystemController system_controller(clock_configuration);

/// Create timer0 to be used by lower level initialization for uptime
/// calculation
sjsu::cortex::DwtCounter arm_dwt_counter;

/// Uart port 0 is used to communicate back to the host computer
sjsu::lpc17xx::Uart & uart0 = sjsu::lpc17xx::GetUart<0>();

/// System timer is used to count milliseconds of time and to run the RTOS
/// scheduler.
sjsu::cortex::SystemTimer system_timer(
    sjsu::lpc40xx::SystemController::Peripherals::kCpu,
    configKERNEL_INTERRUPT_PRIORITY);

/// Platform interrupt controller for Arm Cortex microcontrollers.
sjsu::cortex::InterruptController<sjsu::lpc17xx::kNumberOfIrqs,
                                  __NVIC_PRIO_BITS>
    interrupt_controller;

int Lpc17xxStdOut(std::span<const char> data)
{
  std::span<const uint8_t> char_data(
      reinterpret_cast<const uint8_t *>(data.data()), data.size());

  uart0.Write(char_data);

  return data.size();
}

int Lpc17xxStdIn(std::span<char> data)
{
  std::span<uint8_t> char_data(reinterpret_cast<uint8_t *>(data.data()),
                               data.size());

  // Wait until data comes in
  while (!uart0.HasData())
  {
    continue;
  }

  return uart0.Read(char_data);
}
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
    // Set the SystemTick frequency to the RTOS tick frequency
    // It is critical that this happens before you set the system_clock,
    // since The system_timer keeps the time that the system_clock uses to
    // delay itself.
    system_timer.settings.callback = xPortSysTickHandler;

    // Re-enable timer
    system_timer.Initialize();
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
  // Chip Level - LPC17xx
  interrupt_controller.LookupHandler,  // 16, 0x40 - WDT
  interrupt_controller.LookupHandler,  // 17, 0x44 - TIMER0
  interrupt_controller.LookupHandler,  // 18, 0x48 - TIMER1
  interrupt_controller.LookupHandler,  // 19, 0x4c - TIMER2
  interrupt_controller.LookupHandler,  // 20, 0x50 - TIMER3
  interrupt_controller.LookupHandler,  // 21, 0x54 - UART0
  interrupt_controller.LookupHandler,  // 22, 0x58 - UART1
  interrupt_controller.LookupHandler,  // 23, 0x5c - UART2
  interrupt_controller.LookupHandler,  // 24, 0x60 - UART3
  interrupt_controller.LookupHandler,  // 25, 0x64 - PWM1
  interrupt_controller.LookupHandler,  // 26, 0x68 - I2C0
  interrupt_controller.LookupHandler,  // 27, 0x6c - I2C1
  interrupt_controller.LookupHandler,  // 28, 0x70 - I2C2
  interrupt_controller.LookupHandler,  // 29, 0x74 - SPI0
  interrupt_controller.LookupHandler,  // 30, 0x78 - SSP0
  interrupt_controller.LookupHandler,  // 31, 0x7c - SSP1
  interrupt_controller.LookupHandler,  // 32, 0x80 - PLL0 (Main PLL)
  interrupt_controller.LookupHandler,  // 33, 0x84 - RTC
  interrupt_controller.LookupHandler,  // 34, 0x88 - EINT0
  interrupt_controller.LookupHandler,  // 35, 0x8c - EINT1
  interrupt_controller.LookupHandler,  // 36, 0x90 - EINT2
  interrupt_controller.LookupHandler,  // 37, 0x94 - EINT3
  interrupt_controller.LookupHandler,  // 38, 0x98 - ADC
  interrupt_controller.LookupHandler,  // 39, 0x9c - BOD
  interrupt_controller.LookupHandler,  // 40, 0xA0 - USB
  interrupt_controller.LookupHandler,  // 41, 0xa4 - CAN
  interrupt_controller.LookupHandler,  // 42, 0xa8 - GP DMA
  interrupt_controller.LookupHandler,  // 43, 0xac - I2S
  interrupt_controller.LookupHandler,  // 44, 0xb0 - Ethernet
  interrupt_controller.LookupHandler,  // 45, 0xb4 - RIT
  interrupt_controller.LookupHandler,  // 46, 0xb8 - Motor Control PWM
  interrupt_controller.LookupHandler,  // 47, 0xbc - Quadrature Encoder
  interrupt_controller.LookupHandler,  // 48, 0xc0 - PLL1 (USB PLL)
  interrupt_controller.LookupHandler,  // 49, 0xc4 - USB Activity interrupt to
                                       // wakeup
  interrupt_controller.LookupHandler,  // 50, 0xc8 - CAN Activity interrupt to
                                       // wakeup
};

namespace sjsu
{
SJ2_WEAK(void InitializePlatform());
void InitializePlatform()
{
  // Set the platform's interrupt controller.
  // This will be used by other libraries to enable and disable interrupts.
  sjsu::InterruptController::SetPlatformController(&interrupt_controller);
  sjsu::SystemController::SetPlatformController(&system_controller);

  system_controller.Initialize();

  // Set UART0 baudrate, which is required for printf and scanf to work properly
  uart0.settings.baud_rate = config::kBaudRate;
  uart0.Initialize();

  sjsu::newlib::SetStdout(Lpc17xxStdOut);
  sjsu::newlib::SetStdin(Lpc17xxStdIn);

  system_timer.settings.frequency = config::kRtosFrequency;
  system_timer.Initialize();

  arm_dwt_counter.Initialize();
  sjsu::SetUptimeFunction(sjsu::cortex::SystemTimer::GetCount);
}
}  // namespace sjsu
