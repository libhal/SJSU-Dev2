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
#include <cstdint>
#include <cstring>

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/startup.hpp"
#include "L0_LowLevel/uart0.hpp"
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

// Define an alias for the Isr function pointer.
using IsrPointer = void (*)(void);
// The Interrupt vector table.
// This relies on the linker script to place at correct location in memory.
SJ2_SECTION(".isr_vector")
// NOLINTNEXTLINE(readability-identifier-naming)
const IsrPointer kInterruptVectorTable[] = {
    // Core Level - CM4
    &StackTop,                  // The initial stack pointer
    ResetIsr,                   // The reset handler
    NmiHandler,                 // The NMI handler
    HardFaultHandler,           // The hard fault handler
    MemManageHandler,           // The MPU fault handler
    BusFaultHandler,            // The bus fault handler
    UsageFaultHandler,          // The usage fault handler
    ValidUserCodeChecksum,      // LPC MCU Checksum
    0,                          // Reserved
    0,                          // Reserved
    0,                          // Reserved
    SvcHandler,                 // SVCall handler
    DebugMonHandler,            // Debug monitor handler
    0,                          // Reserved
    PendSVHandler,              // The PendSV handler
    SysTickHandler,             // The SysTick handler
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
    IntDefaultHandler,       // 29, Not used
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

void FreeRtosSystemTick()
{
    // TODO(#101): Add FreeRTOS kernel function here
}

SJ2_WEAK void LowLevelInit()
{
    system_timer.SetIsrFunction(FreeRtosSystemTick);
    system_timer.SetTickFrequency(config::kRtosFrequency);
    bool timer_started_successfully = system_timer.StartTimer();
    if (timer_started_successfully)
    {
        DEBUG_PRINT("System Timer has begun.");
    }
    else
    {
        DEBUG_PRINT("System Timer has FAILED!!");
    }
}

inline void SystemInit()
{
    InitDataSection();
    InitBssSection();
    InitFpu();
#if defined(__cplusplus)
    // Initialisation C++ libraries
    __libc_init_array();
#endif
    // required for printf and scanf to work properly
    uart0::Init(38400);
    LowLevelInit();
}

constexpr uint32_t kNoCrp = 0xFFFFFFFF;
__attribute__((used, section(".crp"))) constexpr uint32_t kCrpWord = kNoCrp;

// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++ library.
void ResetIsr(void)
{
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
    while (true) { continue; }
}

// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
SJ2_SECTION(".after_vectors")
void NmiHandler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void HardFaultHandler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void MemManageHandler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void BusFaultHandler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void UsageFaultHandler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void SvcHandler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void DebugMonHandler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void PendSVHandler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void SysTickHandler(void)
{
    if (SystemTimer::system_timer_isr == nullptr)
    {
        DEBUG_PRINT("System Timer ISR not defined, disabling System Timer\n");
        system_timer.DisableTimer();
    }
    else
    {
        SystemTimer::system_timer_isr();
    }
}
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
void IntDefaultHandler(void)
{
    constexpr int32_t kIrqOffset = 16;
    uint32_t active_isr          = SCB->ICSR;

    void (*isr)(void) = dynamic_isr_vector_table[active_isr - kIrqOffset];
    if (isr == IntDefaultHandler)
    {
        DEBUG_PRINT("No ISR found for the vector %lu\n", active_isr);
        while (1) { continue; }
    }
    else
    {
        DEBUG_PRINT("Launching IRQ %lu\n", active_isr);
        isr();
    }
}
