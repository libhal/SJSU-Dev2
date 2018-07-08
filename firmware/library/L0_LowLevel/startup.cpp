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
#include "L0_LowLevel/uart0.min.hpp"
#include "L2_Utilities/macros.hpp"

// The entry point for the C++ library startup
extern "C"
{
    extern void __libc_init_array(void);
}

#define WEAK __attribute__((weak))
#define ALIAS(f) __attribute__((weak, alias(#f)))

#if defined(__cplusplus)
extern "C"
{
#endif
    extern void SystemInit(void);
    // Forward declaration of the default handlers. These are aliased.
    // When the application defines a handler (with the same name), this will
    // automatically take precedence over these weak definitions
    void ResetISR(void);
    WEAK void NMI_Handler(void);
    WEAK void HardFault_Handler(void);
    WEAK void MemManage_Handler(void);
    WEAK void BusFault_Handler(void);
    WEAK void UsageFault_Handler(void);
    WEAK void SVC_Handler(void);
    WEAK void DebugMon_Handler(void);
    WEAK void PendSV_Handler(void);
    WEAK void SysTick_Handler(void);
    WEAK void IntDefaultHandler(void);
    // Forward declaration of the specific IRQ handlers. These are aliased
    // to the IntDefaultHandler, which is a 'forever' loop. When the application
    // defines a handler (with the same name), this will automatically take
    // precedence over these weak definitions
    void WDT_IRQHandler(void) ALIAS(IntDefaultHandler);
    void TIMER0_IRQHandler(void) ALIAS(IntDefaultHandler);
    void TIMER1_IRQHandler(void) ALIAS(IntDefaultHandler);
    void TIMER2_IRQHandler(void) ALIAS(IntDefaultHandler);
    void TIMER3_IRQHandler(void) ALIAS(IntDefaultHandler);
    void UART0_IRQHandler(void) ALIAS(IntDefaultHandler);
    void UART1_IRQHandler(void) ALIAS(IntDefaultHandler);
    void UART2_IRQHandler(void) ALIAS(IntDefaultHandler);
    void UART3_IRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM1_IRQHandler(void) ALIAS(IntDefaultHandler);
    void I2C0_IRQHandler(void) ALIAS(IntDefaultHandler);
    void I2C1_IRQHandler(void) ALIAS(IntDefaultHandler);
    void I2C2_IRQHandler(void) ALIAS(IntDefaultHandler);
    void SPI_IRQHandler(void) ALIAS(IntDefaultHandler);
    void SSP0_IRQHandler(void) ALIAS(IntDefaultHandler);
    void SSP1_IRQHandler(void) ALIAS(IntDefaultHandler);
    void PLL0_IRQHandler(void) ALIAS(IntDefaultHandler);
    void RTC_IRQHandler(void) ALIAS(IntDefaultHandler);
    void EINT0_IRQHandler(void) ALIAS(IntDefaultHandler);
    void EINT1_IRQHandler(void) ALIAS(IntDefaultHandler);
    void EINT2_IRQHandler(void) ALIAS(IntDefaultHandler);
    void EINT3_IRQHandler(void) ALIAS(IntDefaultHandler);
    void ADC_IRQHandler(void) ALIAS(IntDefaultHandler);
    void BOD_IRQHandler(void) ALIAS(IntDefaultHandler);
    void USB_IRQHandler(void) ALIAS(IntDefaultHandler);
    void CAN_IRQHandler(void) ALIAS(IntDefaultHandler);
    void DMA_IRQHandler(void) ALIAS(IntDefaultHandler);
    void I2S_IRQHandler(void) ALIAS(IntDefaultHandler);
    void ENET_IRQHandler(void) ALIAS(IntDefaultHandler);
    void RIT_IRQHandler(void) ALIAS(IntDefaultHandler);
    void MCPWM_IRQHandler(void) ALIAS(IntDefaultHandler);
    void QEI_IRQHandler(void) ALIAS(IntDefaultHandler);
    void PLL1_IRQHandler(void) ALIAS(IntDefaultHandler);
    void USBActivity_IRQHandler(void) ALIAS(IntDefaultHandler);
    void CANActivity_IRQHandler(void) ALIAS(IntDefaultHandler);
    void SDIO_IRQHandler(void) ALIAS(IntDefaultHandler);
    void UART4_IRQHandler(void) ALIAS(IntDefaultHandler);
    void SSP2_IRQHandler(void) ALIAS(IntDefaultHandler);
    void LCD_IRQHandler(void) ALIAS(IntDefaultHandler);
    void GPIO_IRQHandler(void) ALIAS(IntDefaultHandler);
    void PWM0_IRQHandler(void) ALIAS(IntDefaultHandler);
    void EEPROM_IRQHandler(void) ALIAS(IntDefaultHandler);
    // The entry point for the application.
    // main() is the entry point for Newlib based applications
    extern int main(void);
    // External declaration for the pointer to the stack top from the Linker
    // Script
    extern void _vStackTop(void);

    // External declaration for LPC MCU vector table checksum from  Linker
    // Script
    WEAK extern void __valid_user_code_checksum();
#if defined(__cplusplus)
}  // extern "C"
#endif
// The Interrupt vector table.
// This relies on the linker script to place at correct location in memory.
extern void (*const g_pfnVectors[])(void);
SJ2_SECTION(".isr_vector")
void (*const g_pfnVectors[])(void) = {
    // Core Level - CM4
    &_vStackTop,                 // The initial stack pointer
    ResetISR,                    // The reset handler
    NMI_Handler,                 // The NMI handler
    HardFault_Handler,           // The hard fault handler
    MemManage_Handler,           // The MPU fault handler
    BusFault_Handler,            // The bus fault handler
    UsageFault_Handler,          // The usage fault handler
    __valid_user_code_checksum,  // LPC MCU Checksum
    0,                           // Reserved
    0,                           // Reserved
    0,                           // Reserved
    SVC_Handler,                 // SVCall handler
    DebugMon_Handler,            // Debug monitor handler
    0,                           // Reserved
    PendSV_Handler,              // The PendSV handler
    SysTick_Handler,             // The SysTick handler
    // Chip Level - LPC40xx
    WDT_IRQHandler,          // 16, 0x40 - WDT
    TIMER0_IRQHandler,       // 17, 0x44 - TIMER0
    TIMER1_IRQHandler,       // 18, 0x48 - TIMER1
    TIMER2_IRQHandler,       // 19, 0x4c - TIMER2
    TIMER3_IRQHandler,       // 20, 0x50 - TIMER3
    UART0_IRQHandler,        // 21, 0x54 - UART0
    UART1_IRQHandler,        // 22, 0x58 - UART1
    UART2_IRQHandler,        // 23, 0x5c - UART2
    UART3_IRQHandler,        // 24, 0x60 - UART3
    PWM1_IRQHandler,         // 25, 0x64 - PWM1
    I2C0_IRQHandler,         // 26, 0x68 - I2C0
    I2C1_IRQHandler,         // 27, 0x6c - I2C1
    I2C2_IRQHandler,         // 28, 0x70 - I2C2
    IntDefaultHandler,       // 29, Not used
    SSP0_IRQHandler,         // 30, 0x78 - SSP0
    SSP1_IRQHandler,         // 31, 0x7c - SSP1
    PLL0_IRQHandler,         // 32, 0x80 - PLL0 (Main PLL)
    RTC_IRQHandler,          // 33, 0x84 - RTC
    EINT0_IRQHandler,        // 34, 0x88 - EINT0
    EINT1_IRQHandler,        // 35, 0x8c - EINT1
    EINT2_IRQHandler,        // 36, 0x90 - EINT2
    EINT3_IRQHandler,        // 37, 0x94 - EINT3
    ADC_IRQHandler,          // 38, 0x98 - ADC
    BOD_IRQHandler,          // 39, 0x9c - BOD
    USB_IRQHandler,          // 40, 0xA0 - USB
    CAN_IRQHandler,          // 41, 0xa4 - CAN
    DMA_IRQHandler,          // 42, 0xa8 - GP DMA
    I2S_IRQHandler,          // 43, 0xac - I2S
    ENET_IRQHandler,         // 44, 0xb0 - Ethernet
    SDIO_IRQHandler,         // 45, 0xb4 - SD/MMC card I/F
    MCPWM_IRQHandler,        // 46, 0xb8 - Motor Control PWM
    QEI_IRQHandler,          // 47, 0xbc - Quadrature Encoder
    PLL1_IRQHandler,         // 48, 0xc0 - PLL1 (USB PLL)
    USBActivity_IRQHandler,  // 49, 0xc4 - USB Activity interrupt to wakeup
    CANActivity_IRQHandler,  // 50, 0xc8 - CAN Activity interrupt to wakeup
    UART4_IRQHandler,        // 51, 0xcc - UART4
    SSP2_IRQHandler,         // 52, 0xd0 - SSP2
    LCD_IRQHandler,          // 53, 0xd4 - LCD
    GPIO_IRQHandler,         // 54, 0xd8 - GPIO
    PWM0_IRQHandler,         // 55, 0xdc - PWM0
    EEPROM_IRQHandler,       // 56, 0xe0 - EEPROM
};

// .data Section Table Information
extern struct DataSectionTable_t
{
    uint32_t * rom_location;
    uint32_t * ram_location;
    uint32_t length;
} __attribute__((packed)) __data_section_table[];
extern DataSectionTable_t __data_section_table_end;

// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with MCUs with multiple banks of
// memory.
SJ2_SECTION(".after_vectors")
void InitDataSection()
{
    for (int i = 0; &__data_section_table[i] < &__data_section_table_end; i++)
    {
        uint32_t * rom_location = __data_section_table[i].rom_location;
        uint32_t * ram_location = __data_section_table[i].ram_location;
        uint32_t length         = __data_section_table[i].length << 2;
        memcpy(ram_location, rom_location, length);
    }
}

// .bss Section Table Information
extern struct BssSectionTable_t
{
    uint32_t * ram_location;
    uint32_t length;
} __attribute__((packed)) __bss_section_table[];
extern BssSectionTable_t __bss_section_table_end;

// Functions to initialization BSS data sections. This is important because
// the std c libs assume that BSS is set to zero.
SJ2_SECTION(".after_vectors")
void InitBssSection()
{
    for (int i = 0; &__bss_section_table[i] < &__bss_section_table_end; i++)
    {
        uint32_t * ram_location = __bss_section_table[i].ram_location;
        uint32_t length         = __bss_section_table[i].length << 2;
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
    uart0_init(38400);
}

constexpr uint32_t CRP_NO_CRP                                      = 0xFFFFFFFF;
__attribute__((used, section(".crp"))) constexpr uint32_t CRP_WORD = CRP_NO_CRP;

// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++ library.
SJ2_SECTION(".after_vectors")
void ResetISR(void)
{
    SystemInit();
// #pragma ignored "-Wpedantic" to suppress main function call warning
#pragma GCC diagnostic ignored "-Wpedantic"
    int32_t result = main();
// Enforce the warning after this point
#pragma GCC diagnostic pop
    // Get rid of unused warning.
    (void)result;
    // main() shouldn't return, but if it does, we'll just enter an infinite
    // loop
    while (true) { continue; }
}

// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
SJ2_SECTION(".after_vectors")
void NMI_Handler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void HardFault_Handler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void MemManage_Handler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void BusFault_Handler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void UsageFault_Handler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void SVC_Handler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void DebugMon_Handler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void PendSV_Handler(void)
{
    while (1) { continue; }
}

SJ2_SECTION(".after_vectors")
void SysTick_Handler(void)
{
    while (1) { continue; }
}
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
SJ2_SECTION(".after_vectors")
void IntDefaultHandler(void)
{
    while (1) { continue; }
}
