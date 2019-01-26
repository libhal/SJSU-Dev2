//*****************************************************************************
// LPC407x_8x Microcontroller Startup code for use with LPCXpresso IDE
//
// Version : 150706
//*****************************************************************************
//
// Copyright(C) NXP Semiconductors, 2014-2015
// All rights reserved.
//
// Software that is described herein is for illustrative purposes only
// which provides customers with programming information regarding the
// LPC products.  This software is supplied "AS IS" without any warranties of
// any kind, and NXP Semiconductors and its licensor disclaim any and
// all warranties, express or implied, including all implied warranties of
// merchantability, fitness for a particular purpose and non-infringement of
// intellectual property rights.  NXP Semiconductors assumes no responsibility
// or liability for the use of the software, conveys no license or rights under
// any patent, copyright, mask work right, or any other intellectual property
// rights in or to any products. NXP Semiconductors reserves the right to make
// changes in the software without notification. NXP Semiconductors also makes
// no representation or warranty that such application will be suitable for the
// specified use without further testing or modification.
//
// Permission to use, copy, modify, and distribute this software and its
// documentation is hereby granted, under NXP Semiconductors' and its
// licensor's relevant copyrights in the software, without fee, provided that it
// is used in conjunction with NXP Semiconductors microcontrollers.  This
// copyright, permission, and disclaimer notice must appear in all copies of
// this code.
//*****************************************************************************
#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>
#include <cstring>
#include <iterator>

#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/ram.hpp"
#include "L0_LowLevel/system_controller.hpp"
#include "L1_Drivers/system_timer.hpp"
#include "L1_Drivers/timer.hpp"
#include "L1_Drivers/uart.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"

// Private namespace to make sure that these do not conflict with other globals
namespace
{
// Create system timer to be used by low level initialization.
SystemTimer system_timer;
// Create LPC40xx system controller to be used by low level initialization.
Lpc40xxSystemController system_controller;
// Create timer0 to be used by lower level initialization for uptime calculation
Timer timer0(Timer::TimerPort::kTimer0);
uint64_t Lpc40xxUptime()
{
  return timer0.GetTimer();
}
}  // namespace

extern "C" uint64_t UptimeRTOS()
{
  return timer0.GetTimer();
}
// The entry point for the C++ library startup
extern "C"
{
  // NOLINTNEXTLINE(readability-identifier-naming)
  extern void __libc_init_array(void);
  // The entry point for the application.
  // main() is the entry point for newlib based applications
  extern int main(void);
  // Implementation of vApplicationGetIdleTaskMemory required when
  // configSUPPORT_STATIC_ALLOCATION == 1.
  // The function is called to statically create the idle task when
  // vTaskStartScheduler is invoked.
  static StaticTask_t idle_task_tcb;
  static StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
  void vApplicationGetIdleTaskMemory(  // NOLINT
      StaticTask_t ** ppx_idle_task_tcb_buffer,
      StackType_t ** ppx_idle_task_stack_buffer,
      uint32_t * pul_idle_task_stack_size)
  {
    *ppx_idle_task_tcb_buffer   = &idle_task_tcb;
    *ppx_idle_task_stack_buffer = idle_task_stack;
    *pul_idle_task_stack_size   = std::size(idle_task_stack);
  }
  void vPortSetupTimerInterrupt(void)  // NOLINT
  {
    system_timer.DisableTimer();
    system_timer.SetTickFrequency(config::kRtosFrequency);
    system_timer.SetIsrFunction(xPortSysTickHandler);
    system_timer.StartTimer();
  }
}

SJ2_IGNORE_STACK_TRACE(void InitializeFreeRTOSSystemTick());
SJ2_IGNORE_STACK_TRACE(void SetupTimerInterrupt());
SJ2_IGNORE_STACK_TRACE(void InitDataSection());
SJ2_IGNORE_STACK_TRACE(void InitBssSection());
SJ2_IGNORE_STACK_TRACE(void InitFpu());
SJ2_IGNORE_STACK_TRACE(void __libc_init_array());
SJ2_IGNORE_STACK_TRACE(void LowLevelInit());
SJ2_WEAK(void LowLevelInit());
SJ2_IGNORE_STACK_TRACE(void SystemInit());

// Functions to carry out the initialization of RW and BSS data sections.
SJ2_SECTION(".after_vectors")
void InitDataSection()
{
  for (int i = 0; &data_section_table[i] < &data_section_table_end; i++)
  {
    uint32_t * rom_location = data_section_table[i].rom_location;
    uint32_t * ram_location = data_section_table[i].ram_location;
    uint32_t length         = data_section_table[i].length;
    for (size_t j = 0; j < length; j++)
    {
      ram_location[j] = rom_location[j];
    }
  }
}

// Functions to initialization BSS data sections. This is important because
// the std c libs assume that BSS is set to zero.
SJ2_SECTION(".after_vectors")
void InitBssSection()
{
  for (int i = 0; &bss_section_table[i] < &bss_section_table_end; i++)
  {
    uint32_t * ram_location = bss_section_table[i].ram_location;
    uint32_t length         = bss_section_table[i].length;
    for (size_t j = 0; j < length; j++)
    {
      ram_location[j] = 0;
    }
  }
}

// Initialize the FPU. Must be done before any floating point instructions
// are executed.
/* Found here:
http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/
BABGHFIB.html
*/
SJ2_SECTION(".after_vectors")
void InitFpu()
{
  __asm(
      // CPACR is located at address 0xE000ED88
      "LDR.W   R0, =0xE000ED88\n"
      // Read CPACR
      "LDR     R1, [R0]\n"
      // Set bits 20-23 to enable CP10 and CP11 coprocessors
      "ORR     R1, R1, #(0xF << 20)\n"
      // Write back the modified value to the CPACR
      "STR     R1, [R0]\n"
      // Wait for store to complete
      "DSB\n"
      // reset pipeline now the FPU is enabled
      "ISB\n");
}

void LowLevelInit()
{
  // Set the SystemTick frequency to the RTOS tick frequency
  // It is critical that this happens before you set the system_clock, since
  // The system_timer keeps the time that the system_clock uses to delay itself.
  system_timer.SetTickFrequency(config::kRtosFrequency);
  bool timer_started_successfully = system_timer.StartTimer();
  SJ2_ASSERT_WARNING(timer_started_successfully,
                     "System Timer has FAILED to start!");
  // Set Clock Speed
  // SetClockFrequency will timeout return the offset between desire clockspeed
  // and actual clockspeed if the PLL doesn't get a frequency fix within a
  // defined timeout (see L1/system_clock.hpp:kDefaultTimeout)
  while (system_controller.SetClockFrequency(config::kSystemClockRateMhz) != 0)
  {
    // Continually attempt to set the clock frequency to the desired until the
    // delta between desired and actual are 0.
    continue;
  }
  // Enable Peripheral Clock and set its divider to 1 meaning the clock speed
  // fed to all peripherals will be 48Mhz.
  system_controller.SetPeripheralClockDivider(1);
  // Set System Timer frequency again, since the clock speed has changed since
  // the last time we ran this.
  system_timer.DisableTimer();
  system_timer.SetTickFrequency(config::kRtosFrequency);
  system_timer.StartTimer();
  // Set timer0 to 1 MHz (1,000,000 Hz) so that the timer increments every 1
  // micro second.
  timer0.Initialize(1'000'000);
  SetUptimeFunction(Lpc40xxUptime);
  // Set UART0 baudrate, which is required for printf and scanf to work properly
  uart0.Initialize(config::kBaudRate);
}

void SystemInit()
{
  // Transfer data section values from flash to RAM
  InitDataSection();
  // Clear BSS section of RAM
  // This is required because the nano implementation of the standard C/C++
  // libraries assumes that the BSS section is initialized to 0.
  InitBssSection();
  // Enable FPU (F.loating P.oint U.nit)
  // System will crash if floating point operations occur without
  // Initializing the FPU.
  InitFpu();
  // Initialisation C++ libraries
  __libc_init_array();
  // Run LowLevel System Initialization
  LowLevelInit();
}

SJ2_SECTION(".crp") constexpr uint32_t kCrpWord = 0xFFFFFFFF;
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++ library.

extern "C"
{
  void ResetIsr(void)
  {
    // The Hyperload bootloader takes up stack space to execute. The Hyperload
    // bootloader function launches this ISR manually, but it never returns thus
    // it never cleans up the memory it uses. To get that memory back, we have
    // to manually move the stack pointers back to the top of stack.
    const uint32_t kTopOfStack = reinterpret_cast<intptr_t>(&StackTop);
    __set_PSP(kTopOfStack);
    __set_MSP(kTopOfStack);

    SystemInit();
// #pragma ignored "-Wpedantic" to suppress main function call warning
#pragma GCC diagnostic push ignored "-Wpedantic"
    [[maybe_unused]] int32_t result = main();
// Enforce the warning after this point
#pragma GCC diagnostic pop
    // main() shouldn't return, but if it does, we'll just enter an infinite
    // loop
    Halt();
  }
}
