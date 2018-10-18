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
#include "startup.hpp"

#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>
#include <cstring>

#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/startup.hpp"
#include "L0_LowLevel/uart0.hpp"
#include "L1_Drivers/system_clock.hpp"
#include "L1_Drivers/system_timer.hpp"
#include "L2_Utilities/debug_print.hpp"
#include "L2_Utilities/macros.hpp"

// The entry point for the C++ library startup
extern "C"
{
  // NOLINTNEXTLINE(readability-identifier-naming)
  extern void __libc_init_array(void);
}

#if defined(__cplusplus)
extern "C"
{
#endif
  extern void SystemInit(void);
  // Forward declaration of the default handlers. These are aliased.
  // When the application defines a handler (with the same name), this will
  // automatically take precedence over these weak definitions
  void ResetIsr(void);
  SJ2_WEAK void NmiHandler(void);
  SJ2_WEAK void HardFaultHandler(void);
  SJ2_WEAK void MemManageHandler(void);
  SJ2_WEAK void BusFaultHandler(void);
  SJ2_WEAK void UsageFaultHandler(void);
  SJ2_WEAK void SvcHandler(void);
  SJ2_WEAK void DebugMonHandler(void);
  SJ2_WEAK void PendSVHandler(void);
  SJ2_WEAK void SysTickHandler(void);
  SJ2_WEAK void IntDefaultHandler(void);
  // Forward declaration of the specific IRQ handlers. These are aliased
  // to the IntDefaultHandler, which is a 'forever' loop. When the application
  // defines a handler (with the same name), this will automatically take
  // precedence over these weak definitions
  void WdtIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Timer0IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Timer1IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Timer2IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Timer3IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Uart0IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Uart1IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Uart2IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Uart3IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Pwm1IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void I2c0IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void I2c1IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void I2c2IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void SpiIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Ssp0IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Ssp1IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Pll0IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void RtcIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Eint0IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Eint1IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Eint2IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Eint3IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void AdcIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void BodIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void UsbIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void CanIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void DmaIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void I2sIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void EnetIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void RitIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void McpwmIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void QeiIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Pll1IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void UsbactivityIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void CanactivityIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void SdioIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Uart4IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Ssp2IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void LcdIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void GpioIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void Pwm0IrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  void EepromIrqHandler(void) SJ2_ALIAS(IntDefaultHandler);
  // The entry point for the application.
  // main() is the entry point for Newlib based applications
  extern int main(void);
  // External declaration for the pointer to the stack top from the Linker
  // Script
  extern void StackTop(void);

  // External declaration for LPC MCU vector table checksum from  Linker
  // Script
  SJ2_WEAK extern void ValidUserCodeChecksum();
#if defined(__cplusplus)
}  // extern "C"
#endif

// The Interrupt vector table.
// This relies on the linker script to place at correct location in memory.
SJ2_SECTION(".isr_vector")
// NOLINTNEXTLINE(readability-identifier-naming)
const IsrPointer kInterruptVectorTable[] = {
  // Core Level - CM4
  &StackTop,              // The initial stack pointer
  ResetIsr,               // The reset handler
  NmiHandler,             // The NMI handler
  HardFaultHandler,       // The hard fault handler
  MemManageHandler,       // The MPU fault handler
  BusFaultHandler,        // The bus fault handler
  UsageFaultHandler,      // The usage fault handler
  ValidUserCodeChecksum,  // LPC MCU Checksum
  0,                      // Reserved
  0,                      // Reserved
  0,                      // Reserved
  vPortSVCHandler,        // SVCall handler
  DebugMonHandler,        // Debug monitor handler
  0,                      // Reserved
  xPortPendSVHandler,     // FreeRTOS PendSV Handler
  SysTickHandler,         // The SysTick handler
  // Chip Level - LPC40xx
  WdtIrqHandler,          // 16, 0x40 - WDT
  Timer0IrqHandler,       // 17, 0x44 - TIMER0
  Timer1IrqHandler,       // 18, 0x48 - TIMER1
  Timer2IrqHandler,       // 19, 0x4c - TIMER2
  Timer3IrqHandler,       // 20, 0x50 - TIMER3
  Uart0IrqHandler,        // 21, 0x54 - UART0
  Uart1IrqHandler,        // 22, 0x58 - UART1
  Uart2IrqHandler,        // 23, 0x5c - UART2
  Uart3IrqHandler,        // 24, 0x60 - UART3
  Pwm1IrqHandler,         // 25, 0x64 - PWM1
  I2c0IrqHandler,         // 26, 0x68 - I2C0
  I2c1IrqHandler,         // 27, 0x6c - I2C1
  I2c2IrqHandler,         // 28, 0x70 - I2C2
  IntDefaultHandler,      // 29, Not used
  Ssp0IrqHandler,         // 30, 0x78 - SSP0
  Ssp1IrqHandler,         // 31, 0x7c - SSP1
  Pll0IrqHandler,         // 32, 0x80 - PLL0 (Main PLL)
  RtcIrqHandler,          // 33, 0x84 - RTC
  Eint0IrqHandler,        // 34, 0x88 - EINT0
  Eint1IrqHandler,        // 35, 0x8c - EINT1
  Eint2IrqHandler,        // 36, 0x90 - EINT2
  Eint3IrqHandler,        // 37, 0x94 - EINT3
  AdcIrqHandler,          // 38, 0x98 - ADC
  BodIrqHandler,          // 39, 0x9c - BOD
  UsbIrqHandler,          // 40, 0xA0 - USB
  CanIrqHandler,          // 41, 0xa4 - CAN
  DmaIrqHandler,          // 42, 0xa8 - GP DMA
  I2sIrqHandler,          // 43, 0xac - I2S
  EnetIrqHandler,         // 44, 0xb0 - Ethernet
  SdioIrqHandler,         // 45, 0xb4 - SD/MMC card I/F
  McpwmIrqHandler,        // 46, 0xb8 - Motor Control PWM
  QeiIrqHandler,          // 47, 0xbc - Quadrature Encoder
  Pll1IrqHandler,         // 48, 0xc0 - PLL1 (USB PLL)
  UsbactivityIrqHandler,  // 49, 0xc4 - USB Activity interrupt to wakeup
  CanactivityIrqHandler,  // 50, 0xc8 - CAN Activity interrupt to wakeup
  Uart4IrqHandler,        // 51, 0xcc - UART4
  Ssp2IrqHandler,         // 52, 0xd0 - SSP2
  LcdIrqHandler,          // 53, 0xd4 - LCD
  GpioIrqHandler,         // 54, 0xd8 - GPIO
  Pwm0IrqHandler,         // 55, 0xdc - PWM0
  EepromIrqHandler,       // 56, 0xe0 - EEPROM
};

IsrPointer dynamic_isr_vector_table[] = {
  // Chip Level - LPC40xx
  WdtIrqHandler,          // 16, 0x40 - WDT
  Timer0IrqHandler,       // 17, 0x44 - TIMER0
  Timer1IrqHandler,       // 18, 0x48 - TIMER1
  Timer2IrqHandler,       // 19, 0x4c - TIMER2
  Timer3IrqHandler,       // 20, 0x50 - TIMER3
  Uart0IrqHandler,        // 21, 0x54 - UART0
  Uart1IrqHandler,        // 22, 0x58 - UART1
  Uart2IrqHandler,        // 23, 0x5c - UART2
  Uart3IrqHandler,        // 24, 0x60 - UART3
  Pwm1IrqHandler,         // 25, 0x64 - PWM1
  I2c0IrqHandler,         // 26, 0x68 - I2C0
  I2c1IrqHandler,         // 27, 0x6c - I2C1
  I2c2IrqHandler,         // 28, 0x70 - I2C2
  IntDefaultHandler,      // 29, Not used
  Ssp0IrqHandler,         // 30, 0x78 - SSP0
  Ssp1IrqHandler,         // 31, 0x7c - SSP1
  Pll0IrqHandler,         // 32, 0x80 - PLL0 (Main PLL)
  RtcIrqHandler,          // 33, 0x84 - RTC
  Eint0IrqHandler,        // 34, 0x88 - EINT0
  Eint1IrqHandler,        // 35, 0x8c - EINT1
  Eint2IrqHandler,        // 36, 0x90 - EINT2
  Eint3IrqHandler,        // 37, 0x94 - EINT3
  AdcIrqHandler,          // 38, 0x98 - ADC
  BodIrqHandler,          // 39, 0x9c - BOD
  UsbIrqHandler,          // 40, 0xA0 - USB
  CanIrqHandler,          // 41, 0xa4 - CAN
  DmaIrqHandler,          // 42, 0xa8 - GP DMA
  I2sIrqHandler,          // 43, 0xac - I2S
  EnetIrqHandler,         // 44, 0xb0 - Ethernet
  SdioIrqHandler,         // 45, 0xb4 - SD/MMC card I/F
  McpwmIrqHandler,        // 46, 0xb8 - Motor Control PWM
  QeiIrqHandler,          // 47, 0xbc - Quadrature Encoder
  Pll1IrqHandler,         // 48, 0xc0 - PLL1 (USB PLL)
  UsbactivityIrqHandler,  // 49, 0xc4 - USB Activity interrupt to wakeup
  CanactivityIrqHandler,  // 50, 0xc8 - CAN Activity interrupt to wakeup
  Uart4IrqHandler,        // 51, 0xcc - UART4
  Ssp2IrqHandler,         // 52, 0xd0 - SSP2
  LcdIrqHandler,          // 53, 0xd4 - LCD
  GpioIrqHandler,         // 54, 0xd8 - GPIO
  Pwm0IrqHandler,         // 55, 0xdc - PWM0
  EepromIrqHandler,       // 56, 0xe0 - EEPROM
};

extern "C" void vPortSetupTimerInterrupt(void)  // NOLINT
{
  // Empty implementation, startup handles this itself.
}

static StaticTask_t idle_task_tcb;
static StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
// Implementation of vApplicationGetIdleTaskMemory required when
// configSUPPORT_STATIC_ALLOCATION == 1.
// The function is called to statically create the idle task when
// vTaskStartScheduler is invoked.
extern "C" void vApplicationGetIdleTaskMemory(  // NOLINT
    StaticTask_t ** ppx_idle_task_tcb_buffer,
    StackType_t ** ppx_idle_task_stack_buffer,
    uint32_t * pul_idle_task_stack_size)
{
  *ppx_idle_task_tcb_buffer   = &idle_task_tcb;
  *ppx_idle_task_stack_buffer = idle_task_stack;
  *pul_idle_task_stack_size   = SJ2_ARRAY_LENGTH(idle_task_stack);
}

// .data Section Table Information
SJ2_PACKED(struct)
DataSectionTable_t
{
  uint32_t * rom_location;
  uint32_t * ram_location;
  uint32_t length;
};
extern DataSectionTable_t data_section_table[];
extern DataSectionTable_t data_section_table_end;

// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetIsr() function in order to cope with MCUs with multiple banks of
// memory.
SJ2_SECTION(".after_vectors")
void InitDataSection()
{
  for (int i = 0; &data_section_table[i] < &data_section_table_end; i++)
  {
    uint32_t * rom_location = data_section_table[i].rom_location;
    uint32_t * ram_location = data_section_table[i].ram_location;
    uint32_t length         = data_section_table[i].length << 2;
    memcpy(ram_location, rom_location, length);
  }
}

// .bss Section Table Information
SJ2_PACKED(struct)
BssSectionTable_t
{
  uint32_t * ram_location;
  uint32_t length;
};
extern BssSectionTable_t bss_section_table[];
extern BssSectionTable_t bss_section_table_end;

// Functions to initialization BSS data sections. This is important because
// the std c libs assume that BSS is set to zero.
SJ2_SECTION(".after_vectors")
void InitBssSection()
{
  for (int i = 0; &bss_section_table[i] < &bss_section_table_end; i++)
  {
    uint32_t * ram_location = bss_section_table[i].ram_location;
    uint32_t length         = bss_section_table[i].length << 2;
    memset(ram_location, 0, length);
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

SystemTimer system_timer;
SystemClock system_clock;

void InitializeFreeRTOSSystemTick()
{
  if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState())
  {
    // Swap out the SystemTimer isr with FreeRTOS's xPortSysTickHandler
    system_timer.SetIsrFunction(xPortSysTickHandler);
  }
}

void SetupTimerInterrupt()
{
  system_timer.SetIsrFunction(InitializeFreeRTOSSystemTick);
  system_timer.SetTickFrequency(config::kRtosFrequency);
  bool timer_started_successfully = system_timer.StartTimer();
  SJ2_ASSERT_WARNING(timer_started_successfully,
                     "System Timer has FAILED to start!");
}

SJ2_WEAK void LowLevelInit()
{
  // Set Clock Speed
  system_clock.SetClockFrequency(config::kSystemClockRateMhz);
  // Enable Peripheral Clock
  system_clock.SetPeripheralClockDivider(1);
  // required for printf and scanf to work properly
  uart0::Init(config::kBaudRate);
  SetupTimerInterrupt();
}

inline void SystemInit()
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
  int32_t result = main();
// Enforce the warning after this point
#pragma GCC diagnostic pop
  // Get rid of unused warning.
  SJ2_USED(result);
  // main() shouldn't return, but if it does, we'll just enter an infinite
  // loop
  while (true)
  {
    continue;
  }
}

// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
SJ2_SECTION(".after_vectors")
void NmiHandler(void)
{
  while (1)
  {
    continue;
  }
}

extern "C" void GetRegistersFromStack(uint32_t * fault_stack_address)
{
  // These are volatile to try and prevent the compiler/linker optimizing them
  // away as the variables never actually get used.  If the debugger won't
  // show the values of the variables, make them global my moving their
  // declaration outside of this function.
  volatile uint32_t r0;
  volatile uint32_t r1;
  volatile uint32_t r2;
  volatile uint32_t r3;
  volatile uint32_t r12;
  // Link register.
  volatile uint32_t lr;
  // Program counter.
  volatile uint32_t pc;
  // Program status register.
  volatile uint32_t psr;

  r0 = fault_stack_address[0];
  r1 = fault_stack_address[1];
  r2 = fault_stack_address[2];
  r3 = fault_stack_address[3];

  r12 = fault_stack_address[4];
  lr  = fault_stack_address[5];
  pc  = fault_stack_address[6];
  psr = fault_stack_address[7];

  SJ2_USED(r0);
  SJ2_USED(r1);
  SJ2_USED(r2);
  SJ2_USED(r3);
  SJ2_USED(r12);
  SJ2_USED(lr);
  SJ2_USED(pc);
  SJ2_USED(psr);

  DEBUG_PRINT("r0: 0x%08lX, r1: 0x%08lX, r2: 0x%08lX, r3: 0x%08lX ", r0, r1, r2,
              r3);
  DEBUG_PRINT("r12: 0x%08lX, lr: 0x%08lX, pc: 0x%08lX, psr: 0x%08lX", r12, lr,
              pc, psr);

  SJ2_ASSERT_FATAL(false, "Hard Fault Exception Occured!");
  // When the following line is hit, the variables contain the register values
  // Use a JTAG debugger to inspect these variables
  while (true)
  {
    continue;
  }
}

SJ2_SECTION(".after_vectors")
void HardFaultHandler(void)
{
  __asm volatile(
      " tst lr, #4                                          \n"
      " ite eq                                              \n"
      " mrseq r0, msp                                       \n"
      " mrsne r0, psp                                       \n"
      " ldr r1, [r0, #24]                                   \n"
      " ldr r2, handler2_address_const                      \n"
      " bx r2                                               \n"
      " handler2_address_const: .word GetRegistersFromStack \n");
}

SJ2_SECTION(".after_vectors")
void MemManageHandler(void)
{
  while (1)
  {
    continue;
  }
}

SJ2_SECTION(".after_vectors")
void BusFaultHandler(void)
{
  while (1)
  {
    continue;
  }
}

SJ2_SECTION(".after_vectors")
void UsageFaultHandler(void)
{
  while (1)
  {
    continue;
  }
}

SJ2_SECTION(".after_vectors")
void SvcHandler(void)
{
  while (1)
  {
    continue;
  }
}

SJ2_SECTION(".after_vectors")
void DebugMonHandler(void)
{
  while (1)
  {
    continue;
  }
}

SJ2_SECTION(".after_vectors")
void PendSVHandler(void)
{
  while (1)
  {
    continue;
  }
}

SJ2_SECTION(".after_vectors")
void SysTickHandler(void)
{
  // This assumes that SysTickHandler is called every millisecond.
  // Changing that frequency will distort the milliseconds time.
  milliseconds += 1;
  if (SystemTimer::system_timer_isr == nullptr)
  {
    DEBUG_PRINT("System Timer ISR not defined, disabling System Timer");
    system_timer.DisableTimer();
  }
  else
  {
    SystemTimer::system_timer_isr();
  }
}

constexpr int32_t kIrqOffset = 16;

// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
void IntDefaultHandler(void)
{
  uint8_t active_isr = (SCB->ICSR & 0xFF);

  IsrPointer isr = dynamic_isr_vector_table[(active_isr - kIrqOffset)];
  SJ2_ASSERT_FATAL(isr != IntDefaultHandler,
                   "No ISR found for the vector %u [%ld]", active_isr,
                   ((active_isr - kIrqOffset)));
  isr();
}
