/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/* GPIO - Warrior
 * SPI  - Thief
 * I2C  - Dual wield
 * CAN  - Knight
 * PWM  - Mage
 *      -
 *
 */

#include <string.h>

#include "LPC40xx.h"
extern "C" {
    #include "L0-LowLevel/uart0.min.h"
}
#if defined (__cplusplus)
extern "C" {
// The entry point for the C++ library startup
extern void __libc_init_array(void);
#endif

/// CPU execution begins from this function
void isr_reset(void);

/// The common ISR handler for the chip level interrupts that forwards to the user interrupts
static void isr_forwarder_routine(void);

/// isr_forwarder_routine() will call this function unless user interrupt is registered
void isr_default_handler(void);

/// The hard fault handler
void isr_hard_fault_handler(unsigned long *hardfault_args);

/** @{ Weak ISR handlers; these are over-riden when user defines them elsewhere */
#define WEAK     __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

WEAK void isr_nmi(void);
WEAK void isr_hard_fault(void);
WEAK void isr_mem_fault(void);
WEAK void isr_bus_fault(void);
WEAK void isr_usage_fault(void);
WEAK void isr_debug_mon(void);
WEAK void isr_sys_tick(void);
/** @} */

/** @{ Weak ISR handlers; these are over-riden when the user defines them elsewhere */
void WDT_IRQHandler(void)    ALIAS(isr_default_handler);
void TIMER0_IRQHandler(void) ALIAS(isr_default_handler);
void TIMER1_IRQHandler(void) ALIAS(isr_default_handler);
void TIMER2_IRQHandler(void) ALIAS(isr_default_handler);
void TIMER3_IRQHandler(void) ALIAS(isr_default_handler);
void UART0_IRQHandler(void)  ALIAS(isr_default_handler);
void UART1_IRQHandler(void)  ALIAS(isr_default_handler);
void UART2_IRQHandler(void)  ALIAS(isr_default_handler);
void UART3_IRQHandler(void)  ALIAS(isr_default_handler);
void PWM1_IRQHandler(void)   ALIAS(isr_default_handler);
void I2C0_IRQHandler(void)   ALIAS(isr_default_handler);
void I2C1_IRQHandler(void)   ALIAS(isr_default_handler);
void I2C2_IRQHandler(void)   ALIAS(isr_default_handler);
void SPI_IRQHandler(void)    ALIAS(isr_default_handler);
void SSP0_IRQHandler(void)   ALIAS(isr_default_handler);
void SSP1_IRQHandler(void)   ALIAS(isr_default_handler);
void PLL0_IRQHandler(void)   ALIAS(isr_default_handler);
void RTC_IRQHandler(void)    ALIAS(isr_default_handler);
void EINT0_IRQHandler(void)  ALIAS(isr_default_handler);
void EINT1_IRQHandler(void)  ALIAS(isr_default_handler);
void EINT2_IRQHandler(void)  ALIAS(isr_default_handler);
void EINT3_IRQHandler(void)  ALIAS(isr_default_handler);
void ADC_IRQHandler(void)    ALIAS(isr_default_handler);
void BOD_IRQHandler(void)    ALIAS(isr_default_handler);
void USB_IRQHandler(void)    ALIAS(isr_default_handler);
void CAN_IRQHandler(void)    ALIAS(isr_default_handler);
void DMA_IRQHandler(void)    ALIAS(isr_default_handler);
void I2S_IRQHandler(void)    ALIAS(isr_default_handler);
void ENET_IRQHandler(void)   ALIAS(isr_default_handler);
void RIT_IRQHandler(void)    ALIAS(isr_default_handler);
void MCPWM_IRQHandler(void)  ALIAS(isr_default_handler);
void QEI_IRQHandler(void)    ALIAS(isr_default_handler);
void PLL1_IRQHandler(void)   ALIAS(isr_default_handler);
void USBAct_IRQHandler(void) ALIAS(isr_default_handler);
void CANAct_IRQHandler(void) ALIAS(isr_default_handler);
/** @} */

/** @{ FreeRTOS Interrupt Handlers  */
// extern void xPortSysTickHandler(void);  ///< OS timer or tick interrupt (for time slicing tasks)
// extern void xPortPendSVHandler(void);   ///< Context switch is performed using this interrupt
// extern void vPortSVCHandler(void);      ///< OS "supervisor" call to start first FreeRTOS task
/** @} */

/// Linker script (loader.ld) defines the initial stack pointer that we set at the interrupt vector
extern void _vStackTop(void);

#if defined (__cplusplus)
} // extern "C"
#endif

/** @{ External functions that we will call */
extern void low_level_init(void);
extern void high_level_init(void);
extern int main();
/** @} */

/**
 * CPU interrupt vector table that is loaded at the beginning of the CPU start
 * location by using the linker script that will place it at the isr_vector location.
 * CPU loads the stack pointer and begins execution from Reset vector.
 */
extern void (* const g_pfnVectors[])(void);
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
    // Core Level - CM3
    &_vStackTop,        // The initial stack pointer
    isr_reset,          // The reset handler
    isr_nmi,            // The NMI handler
    isr_hard_fault,     // The hard fault handler
    isr_mem_fault,      // The MPU fault handler
    isr_bus_fault,      // The bus fault handler
    isr_usage_fault,    // The usage fault handler
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    0,                  // Reserved
    // vPortSVCHandler,    // FreeRTOS SVC-call handler (naked function so needs direct call - not a wrapper)
    0,    // FreeRTOS SVC-call handler (naked function so needs direct call - not a wrapper)
    isr_debug_mon,      // Debug monitor handler
    0,                  // Reserved
    // xPortPendSVHandler, // FreeRTOS PendSV handler (naked function so needs direct call - not a wrapper)
    0, // FreeRTOS PendSV handler (naked function so needs direct call - not a wrapper)
    isr_sys_tick,       // FreeRTOS SysTick handler (we enclose inside a wrapper to track OS overhead)

    // Chip Level - LPC17xx - common ISR that will call the real ISR
    isr_forwarder_routine,      // 16, 0x40 - WDT
    isr_forwarder_routine,      // 17, 0x44 - TIMER0
    isr_forwarder_routine,      // 18, 0x48 - TIMER1
    isr_forwarder_routine,      // 19, 0x4c - TIMER2
    isr_forwarder_routine,      // 20, 0x50 - TIMER3
    isr_forwarder_routine,      // 21, 0x54 - UART0
    isr_forwarder_routine,      // 22, 0x58 - UART1
    isr_forwarder_routine,      // 23, 0x5c - UART2
    isr_forwarder_routine,      // 24, 0x60 - UART3
    isr_forwarder_routine,      // 25, 0x64 - PWM1
    isr_forwarder_routine,      // 26, 0x68 - I2C0
    isr_forwarder_routine,      // 27, 0x6c - I2C1
    isr_forwarder_routine,      // 28, 0x70 - I2C2
    isr_forwarder_routine,      // 29, 0x74 - SPI
    isr_forwarder_routine,      // 30, 0x78 - SSP0
    isr_forwarder_routine,      // 31, 0x7c - SSP1
    isr_forwarder_routine,      // 32, 0x80 - PLL0 (Main PLL)
    isr_forwarder_routine,      // 33, 0x84 - RTC
    isr_forwarder_routine,      // 34, 0x88 - EINT0
    isr_forwarder_routine,      // 35, 0x8c - EINT1
    isr_forwarder_routine,      // 36, 0x90 - EINT2
    isr_forwarder_routine,      // 37, 0x94 - EINT3
    isr_forwarder_routine,      // 38, 0x98 - ADC
    isr_forwarder_routine,      // 39, 0x9c - BOD
    isr_forwarder_routine,      // 40, 0xA0 - USB
    isr_forwarder_routine,      // 41, 0xa4 - CAN
    isr_forwarder_routine,      // 42, 0xa8 - GP DMA
    isr_forwarder_routine,      // 43, 0xac - I2S
    isr_forwarder_routine,      // 44, 0xb0 - Ethernet
    isr_forwarder_routine,      // 45, 0xb4 - RITINT
    isr_forwarder_routine,      // 46, 0xb8 - Motor Control PWM
    isr_forwarder_routine,      // 47, 0xbc - Quadrature Encoder
    isr_forwarder_routine,      // 48, 0xc0 - PLL1 (USB PLL)
    isr_forwarder_routine,      // 49, 0xc4 - USB Activity interrupt to wakeup
    isr_forwarder_routine,      // 50, 0xc8 - CAN Activity interrupt to wakeup
};

//*****************************************************************************
// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with MCUs with multiple banks of
// memory.
//*****************************************************************************
extern unsigned int __data_section_table[3];
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void initSRAMDataSection()
{
    // Copy data from FLASH to RAM
    uint32_t * rom_data_source  = reinterpret_cast<uint32_t *>(__data_section_table[0]);
    uint32_t * sram_destination = reinterpret_cast<uint32_t *>(__data_section_table[1]);
    uint32_t length             = __data_section_table[2] >> 2;
    memcpy(rom_data_source, sram_destination, length);
}

//*****************************************************************************
// Code Entry Point : The CPU RESET Handler
// This sets up the system and copies global memory contensts from FLASH to RAM
// and initializes C/C++ environment
//*****************************************************************************
extern "C" {
    __attribute__ ((section(".after_vectors"), naked))
    void isr_reset(void)
    {
        // remove compiler warning
        (void)g_pfnVectors;
        /**
         * The hyperload bootloader sets the MSP/PSP upon a true reset, which is when the
         * LPC40xx (Cortex-M3) sets the values of the stack pointer. But since we are
         * booting after a bootloader, we have to manually setup the stack pointers ourselves.
         */
        const uint32_t topOfStack = (uint32_t) &_vStackTop;
        __set_PSP(topOfStack);
        __set_MSP(topOfStack);

        initSRAMDataSection();

        #if defined (__cplusplus)
            __libc_init_array();    // Call C++ library initialization
        #endif

        // low_level_init();   // Initialize minimal system, such as Clock & UART
        // high_level_init();  // Initialize high level board specific features
        #pragma GCC diagnostic ignored "-Wpedantic"
        int result = main();   // Finally call main()
        #pragma GCC diagnostic pop
        // In case main() exits:
        const uint32_t SYS_CFG_UART0_BPS = 38400;
        uart0_init(SYS_CFG_UART0_BPS);
        //// TODO: Print the result code the serial console below
        (void) result;
        uart0_puts("PROGRAM ENDING ... \n");
        while (1);
    }
}
/**
 * Array of IRQs that the user can register, which we default to the weak ISR handler.
 * The user can either define the real one to override the weak handler, or the user
 * can call the isr_register() API to change the function pointer at this array.
 */
typedef void (*isr_func_t) (void);
static isr_func_t g_isr_array[] = {
        WDT_IRQHandler,         // 16, 0x40 - WDT
        TIMER0_IRQHandler,      // 17, 0x44 - TIMER0
        TIMER1_IRQHandler,      // 18, 0x48 - TIMER1
        TIMER2_IRQHandler,      // 19, 0x4c - TIMER2
        TIMER3_IRQHandler,      // 20, 0x50 - TIMER3
        UART0_IRQHandler,       // 21, 0x54 - UART0
        UART1_IRQHandler,       // 22, 0x58 - UART1
        UART2_IRQHandler,       // 23, 0x5c - UART2
        UART3_IRQHandler,       // 24, 0x60 - UART3
        PWM1_IRQHandler,        // 25, 0x64 - PWM1
        I2C0_IRQHandler,        // 26, 0x68 - I2C0
        I2C1_IRQHandler,        // 27, 0x6c - I2C1
        I2C2_IRQHandler,        // 28, 0x70 - I2C2
        SPI_IRQHandler,         // 29, 0x74 - SPI
        SSP0_IRQHandler,        // 30, 0x78 - SSP0
        SSP1_IRQHandler,        // 31, 0x7c - SSP1
        PLL0_IRQHandler,        // 32, 0x80 - PLL0 (Main PLL)
        RTC_IRQHandler,         // 33, 0x84 - RTC
        EINT0_IRQHandler,       // 34, 0x88 - EINT0
        EINT1_IRQHandler,       // 35, 0x8c - EINT1
        EINT2_IRQHandler,       // 36, 0x90 - EINT2
        EINT3_IRQHandler,       // 37, 0x94 - EINT3
        ADC_IRQHandler,         // 38, 0x98 - ADC
        BOD_IRQHandler,         // 39, 0x9c - BOD
        USB_IRQHandler,         // 40, 0xA0 - USB
        CAN_IRQHandler,         // 41, 0xa4 - CAN
        DMA_IRQHandler,         // 42, 0xa8 - GP DMA
        I2S_IRQHandler,         // 43, 0xac - I2S
        ENET_IRQHandler,        // 44, 0xb0 - Ethernet
        RIT_IRQHandler,         // 45, 0xb4 - RITINT
        MCPWM_IRQHandler,       // 46, 0xb8 - Motor Control PWM
        QEI_IRQHandler,         // 47, 0xbc - Quadrature Encoder
        PLL1_IRQHandler,        // 48, 0xc0 - PLL1 (USB PLL)
        USBAct_IRQHandler,      // 49, 0xc4 - USB Activity interrupt to wakeup
        CANAct_IRQHandler,      // 50, 0xc8 - CAN Activity interrupt to wakeup
};

/**
 * This function allows the user to register a function for the interrupt service routine.
 * Registration of an IRQ is not necessary if the weak ISR has been over-riden.
 */
extern "C" void isr_register(
    IRQn_Type num,
    void (*isr_func_ptr) (void)
)
{
    if (num >= 0) {
        g_isr_array[num] = isr_func_ptr;
    }
}

/**
 * This is the common IRQ handler for the chip (or peripheral) interrupts.  We have this
 * common IRQ here to allow more flexibility for the user to register their interrupt.
 * User can either override the aliased IRQ handler, or use the isr_register() API to
 * register it any of their own functions during runtime.
 */
static void isr_forwarder_routine(void)
{
    /* Inform FreeRTOS run-time counter API that we are inside an ISR such that it
     * won't think that the task is using the CPU.
     */
    // vRunTimeStatIsrEntry();

    /* Get the IRQ number we are in.  Note that ICSR's real ISR bits are offset by 16. */
    const uint32_t isr_num = ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) - 16);
    // vTraceStoreISRBegin(isr_num);

    /* Lookup the function pointer we want to call and make the call */
    const isr_func_t isr_to_service = g_isr_array[isr_num];

    /* If the user has not over-riden the "weak" isr name, or not registerd the new one using
     * isr_register(), then it will point to the isr_default_handler.
     */
    if (isr_default_handler == isr_to_service)
    {
        // u0_dbg_printf("%u IRQ was triggered, but no IRQ service was defined!\n", isr_num);
        uart0_puts("IRQ was triggered, but no IRQ service was defined!\n");
        while(1);
    }
    else
    {
        isr_to_service();
    }
    // vTraceStoreISREnd(0);

    /* Inform FreeRTOS that we have exited the ISR */
    // vRunTimeStatIsrExit();
}


__attribute__ ((section(".after_vectors")))
void isr_hard_fault(void)
{
    __asm("MOVS   R0, #4  \n"
            "MOV    R1, LR  \n"
            "TST    R0, R1  \n"
            "BEQ    _MSP    \n"
            "MRS    R0, PSP \n"
            "B      isr_hard_fault_handler  \n"
            "_MSP:  \n"
            "MRS    R0, MSP \n"
            "B      isr_hard_fault_handler  \n"
    );
}

__attribute__ ((section(".after_vectors"))) void isr_nmi(void)        { uart0_puts("NMI Fault\n"); while(1); }
__attribute__ ((section(".after_vectors"))) void isr_mem_fault(void)  { uart0_puts("Mem Fault\n"); while(1); }
__attribute__ ((section(".after_vectors"))) void isr_bus_fault(void)  { uart0_puts("BUS Fault\n"); while(1); }
__attribute__ ((section(".after_vectors"))) void isr_usage_fault(void){ uart0_puts("Usage Fault\n"); while(1); }
__attribute__ ((section(".after_vectors"))) void isr_debug_mon(void)  { uart0_puts("DBGMON Fault\n"); while(1); }

/// If an IRQ is not registered, we end up at this stub function
__attribute__ ((section(".after_vectors"))) void isr_default_handler(void) { uart0_puts("IRQ not registered!"); while(1); }

/// Wrap the FreeRTOS tick function such that we get a true measure of how much CPU tasks are using
// TODO: remove GCC waring ignore after this function has been implemented
#pragma GCC diagnostic ignored "-Wsuggest-attribute=const"
__attribute__ ((section(".after_vectors"))) void isr_sys_tick(void)
{
    // vRunTimeStatIsrEntry();
    // xPortSysTickHandler();
    // vRunTimeStatIsrExit();
}
#pragma GCC diagnostic pop

/**
 * This is called from the HardFault_HandlerAsm with a pointer the Fault stack as the parameter.
 * We can then read the values from the stack and place them into local variables for ease of reading.
 * We then read the various Fault Status and Address Registers to help decode cause of the fault.
 * The function ends with a BKPT instruction to force control back into the debugger
 */
void isr_hard_fault_handler(unsigned long *hardfault_args)
{
    volatile unsigned int stacked_r0 ;
    volatile unsigned int stacked_r1 ;
    volatile unsigned int stacked_r2 ;
    volatile unsigned int stacked_r3 ;
    volatile unsigned int stacked_r12 ;
    volatile unsigned int stacked_lr ;
    volatile unsigned int stacked_pc ;
    volatile unsigned int stacked_psr ;

    stacked_r0 = hardfault_args[0];
    stacked_r1 = hardfault_args[1];
    stacked_r2 = hardfault_args[2];
    stacked_r3 = hardfault_args[3];
    stacked_r12 = hardfault_args[4];
    stacked_lr = hardfault_args[5];
    stacked_pc = hardfault_args[6];
    stacked_psr = hardfault_args[7];

    // FAULT_EXISTS = FAULT_PRESENT_VAL;
    // FAULT_PC = stacked_pc;
    // FAULT_LR = stacked_lr - 1;
    // FAULT_PSR = stacked_psr;

    // sys_reboot();

    /* Prevent compiler warnings */

    static_cast<void>(stacked_r0);
    static_cast<void>(stacked_r1);
    static_cast<void>(stacked_r2);
    static_cast<void>(stacked_r3);
    static_cast<void>(stacked_r12);
    static_cast<void>(stacked_lr);
    static_cast<void>(stacked_pc);
    static_cast<void>(stacked_psr);
}
