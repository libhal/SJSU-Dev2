
#include <cinttypes>
#include <cstdint>

#include <FreeRTOS.h>

#include "L0_LowLevel/interrupt.hpp"
#include "L2_Utilities/macros.hpp"
#include "L2_Utilities/time.hpp"
#include "L4_Application/globals.hpp"

#if defined HOST_TEST
// NOLINTNEXTLINE(readability-identifier-naming)
IsrPointer dynamic_isr_vector_table[56] = { nullptr };

DEFINE_FAKE_VOID_FUNC(NVIC_EnableIRQ, IRQn_Type);
DEFINE_FAKE_VOID_FUNC(NVIC_DisableIRQ, IRQn_Type);
DEFINE_FAKE_VOID_FUNC(NVIC_SetPriority, IRQn_Type, uint32_t);
#else
extern "C"
{
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
  // External declaration for LPC MCU vector table checksum from Linker Script
  extern void ValidUserCodeChecksum();
}

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
#endif  // defined HOST_TEST

// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
void IntDefaultHandler(void)
{
  constexpr int32_t kIrqOffset = 16;
  uint8_t active_isr = (SCB->ICSR & 0xFF);

  IsrPointer isr = dynamic_isr_vector_table[(active_isr - kIrqOffset)];
  SJ2_ASSERT_FATAL(isr != IntDefaultHandler,
                   "No ISR found for the vector %u [%ld]", active_isr,
                   ((active_isr - kIrqOffset)));
  isr();
}

void RegisterIsr(IRQn_Type irq, IsrPointer isr, bool enable_interrupt,
                 int32_t priority)
{
  dynamic_isr_vector_table[irq] = isr;
  if (enable_interrupt)
  {
    NVIC_EnableIRQ(irq);
  }
  if (priority > -1)
  {
    NVIC_SetPriority(irq, priority);
  }
}

void DeregisterIsr(IRQn_Type irq)
{
  NVIC_DisableIRQ(irq);
  dynamic_isr_vector_table[irq] = nullptr;
}

// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
SJ2_WEAK
SJ2_SECTION(".after_vectors")
void NmiHandler(void)
{
  Halt();
}

extern "C" void GetRegistersFromStack(uint32_t * fault_stack_address)
{
  // These are volatile to try and prevent the compiler/linker optimizing them
  // away as the variables never actually get used.  If the debugger won't
  // show the values of the variables, make them global my moving their
  // declaration outside of this function.
  volatile uint32_t r0  = fault_stack_address[0];
  volatile uint32_t r1  = fault_stack_address[1];
  volatile uint32_t r2  = fault_stack_address[2];
  volatile uint32_t r3  = fault_stack_address[3];
  volatile uint32_t r12 = fault_stack_address[4];
  // Link register.
  volatile uint32_t lr = fault_stack_address[5];
  // Program counter.
  volatile uint32_t pc = fault_stack_address[6];
  // Program status register.
  volatile uint32_t psr = fault_stack_address[7];

  DEBUG_PRINT("r0: 0x%08" PRIX32 ", r1: 0x%08" PRIX32
              ", "
              "r2: 0x%08" PRIX32 ", r3: 0x%08" PRIX32 " ",
              r0, r1, r2, r3);
  DEBUG_PRINT("r12: 0x%08" PRIX32 ", lr: 0x%08" PRIX32
              ", "
              "pc: 0x%08" PRIX32 ", psr: 0x%08" PRIX32 "",
              r12, lr, pc, psr);

  SJ2_ASSERT_FATAL(false, "Hard Fault Exception Occured!");
  // When the following line is hit, the variables contain the register values
  // Use a JTAG debugger to inspect these variables
  Halt();
}

SJ2_SECTION(".after_vectors")
void HardFaultHandler(void)
{
#if defined(__arm__)
  __asm volatile(
      " tst lr, #4                                          \n"
      " ite eq                                              \n"
      " mrseq r0, msp                                       \n"
      " mrsne r0, psp                                       \n"
      " ldr r1, [r0, #24]                                   \n"
      " ldr r2, handler2_address_const                      \n"
      " bx r2                                               \n"
      " handler2_address_const: .word GetRegistersFromStack \n");
#endif
}

SJ2_WEAK
SJ2_SECTION(".after_vectors")
void MemManageHandler(void)
{
  Halt();
}

SJ2_WEAK
SJ2_SECTION(".after_vectors")
void BusFaultHandler(void)
{
  Halt();
}

SJ2_WEAK
SJ2_SECTION(".after_vectors")
void UsageFaultHandler(void)
{
  Halt();
}

SJ2_WEAK
SJ2_SECTION(".after_vectors")
void SvcHandler(void)
{
  Halt();
}

SJ2_WEAK
SJ2_SECTION(".after_vectors")
void DebugMonHandler(void)
{
  Halt();
}

SJ2_WEAK
SJ2_SECTION(".after_vectors")
void PendSVHandler(void)
{
  Halt();
}

SJ2_SECTION(".after_vectors")
void SysTickHandler(void)
{
  // This assumes that SysTickHandler is called every millisecond.
  // Changing that frequency will distort the milliseconds time.
  IncrementUptimeMs();
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
