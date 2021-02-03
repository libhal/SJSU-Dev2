/******************************************************************************
*
* Copyright (C) 2012 - 2015 Texas Instruments Incorporated - http://www.ti.com/
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
*  Redistributions of source code must retain the above copyright
*  notice, this list of conditions and the following disclaimer.
*
*  Redistributions in binary form must reproduce the above copyright
*  notice, this list of conditions and the following disclaimer in the
*  documentation and/or other materials provided with the
*  distribution.
*
*  Neither the name of Texas Instruments Incorporated nor the names of
*  its contributors may be used to endorse or promote products derived
*  from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* MSP432P401R Register Definitions
*
* This file includes CMSIS compliant component and register definitions
*
* For legacy components the definitions that are compatible with MSP430 code,
* are included with msp432p401r_classic.h
*
* With CMSIS definitions, the register defines have been reformatted:
*     ModuleName[ModuleInstance]->RegisterName
*
* Writing to CMSIS bit fields can be done through register level
* or via bitband area access:
*  - ADC14->CTL0 |= ADC14_CTL0_ENC;
*  - BITBAND_PERI(ADC14->CTL0, ADC14_CTL0_ENC_OFS) = 1;
*
* File creation date: 2015-10-26
*
******************************************************************************/
#pragma once

#ifndef __MSP432P401R_H__
#define __MSP432P401R_H__

#pragma GCC system_header

/* Use standard integer types with explicit width */
#include <cstdint>

#include "platforms/processors/arm_cortex/m4/core_cm4.h"

#if defined (__cplusplus)
// SJSU-Dev2: Putting contents of this include in sjsu::msp432p401r
namespace sjsu::msp432p401r
{
extern "C" {
#endif

#define __MSP432_HEADER_VERSION__ 2000

#ifndef __CMSIS_CONFIG__
#define __CMSIS_CONFIG__

/** @addtogroup MSP432P401R_Definitions MSP432P401R Definitions
  This file defines all structures and symbols for MSP432P401R:
    - components and registers
    - peripheral base address
    - peripheral ID
    - Peripheral definitions
  @{
*/

/******************************************************************************
*                Processor and Core Peripherals                               *
******************************************************************************/
/** @addtogroup MSP432P401R_CMSIS Device CMSIS Definitions
  Configuration of the Cortex-M4 Processor and Core Peripherals
  @{
*/

/******************************************************************************
* CMSIS-compatible Interrupt Number Definition                                *
******************************************************************************/
enum IRQn
{
  /* Cortex-M4 Processor Exceptions Numbers */
  NonMaskableInt_IRQn   = -14,  /*  2 Non Maskable Interrupt */
  HardFault_IRQn        = -13,  /*  3 Hard Fault Interrupt */
  MemoryManagement_IRQn = -12,  /*  4 Memory Management Interrupt */
  BusFault_IRQn         = -11,  /*  5 Bus Fault Interrupt */
  UsageFault_IRQn       = -10,  /*  6 Usage Fault Interrupt */
  SVCall_IRQn           = -5,   /* 11 SV Call Interrupt */
  DebugMonitor_IRQn     = -4,   /* 12 Debug Monitor Interrupt */
  PendSV_IRQn           = -2,   /* 14 Pend SV Interrupt */
  SysTick_IRQn          = -1,   /* 15 System Tick Interrupt */
  /*  Peripheral Exceptions Numbers */
  PSS_IRQn              =  0,   /* 16 PSS Interrupt */
  CS_IRQn               =  1,   /* 17 CS Interrupt */
  PCM_IRQn              =  2,   /* 18 PCM Interrupt */
  WDT_A_IRQn            =  3,   /* 19 WDT_A Interrupt */
  FPU_IRQn              =  4,   /* 20 FPU Interrupt */
  FLCTL_IRQn            =  5,   /* 21 FLCTL Interrupt */
  COMP_E0_IRQn          =  6,   /* 22 COMP_E0 Interrupt */
  COMP_E1_IRQn          =  7,   /* 23 COMP_E1 Interrupt */
  TA0_0_IRQn            =  8,   /* 24 TA0_0 Interrupt */
  TA0_N_IRQn            =  9,   /* 25 TA0_N Interrupt */
  TA1_0_IRQn            = 10,   /* 26 TA1_0 Interrupt */
  TA1_N_IRQn            = 11,   /* 27 TA1_N Interrupt */
  TA2_0_IRQn            = 12,   /* 28 TA2_0 Interrupt */
  TA2_N_IRQn            = 13,   /* 29 TA2_N Interrupt */
  TA3_0_IRQn            = 14,   /* 30 TA3_0 Interrupt */
  TA3_N_IRQn            = 15,   /* 31 TA3_N Interrupt */
  EUSCIA0_IRQn          = 16,   /* 32 EUSCIA0 Interrupt */
  EUSCIA1_IRQn          = 17,   /* 33 EUSCIA1 Interrupt */
  EUSCIA2_IRQn          = 18,   /* 34 EUSCIA2 Interrupt */
  EUSCIA3_IRQn          = 19,   /* 35 EUSCIA3 Interrupt */
  EUSCIB0_IRQn          = 20,   /* 36 EUSCIB0 Interrupt */
  EUSCIB1_IRQn          = 21,   /* 37 EUSCIB1 Interrupt */
  EUSCIB2_IRQn          = 22,   /* 38 EUSCIB2 Interrupt */
  EUSCIB3_IRQn          = 23,   /* 39 EUSCIB3 Interrupt */
  ADC14_IRQn            = 24,   /* 40 ADC14 Interrupt */
  T32_INT1_IRQn         = 25,   /* 41 T32_INT1 Interrupt */
  T32_INT2_IRQn         = 26,   /* 42 T32_INT2 Interrupt */
  T32_INTC_IRQn         = 27,   /* 43 T32_INTC Interrupt */
  AES256_IRQn           = 28,   /* 44 AES256 Interrupt */
  RTC_C_IRQn            = 29,   /* 45 RTC_C Interrupt */
  DMA_ERR_IRQn          = 30,   /* 46 DMA_ERR Interrupt */
  DMA_INT3_IRQn         = 31,   /* 47 DMA_INT3 Interrupt */
  DMA_INT2_IRQn         = 32,   /* 48 DMA_INT2 Interrupt */
  DMA_INT1_IRQn         = 33,   /* 49 DMA_INT1 Interrupt */
  DMA_INT0_IRQn         = 34,   /* 50 DMA_INT0 Interrupt */
  PORT1_IRQn            = 35,   /* 51 PORT1 Interrupt */
  PORT2_IRQn            = 36,   /* 52 PORT2 Interrupt */
  PORT3_IRQn            = 37,   /* 53 PORT3 Interrupt */
  PORT4_IRQn            = 38,   /* 54 PORT4 Interrupt */
  PORT5_IRQn            = 39,   /* 55 PORT5 Interrupt */
  PORT6_IRQn            = 40,   /* 56 PORT6 Interrupt */
  kNumberOfIrqs
};

/******************************************************************************
* Processor and Core Peripheral Section                                       *
******************************************************************************/
#define __CM4_REV               0x000 /* Core revision r0p1 */
#define __MPU_PRESENT           1     /* MPU present or not */
#define __NVIC_PRIO_BITS        3     /* Number of Bits used for Prio Levels */
#define __Vendor_SysTickConfig  0     /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT           1     /* FPU present or not */

/******************************************************************************
* Available Peripherals                                                       *
******************************************************************************/
#define __MCU_HAS_ADC14__     /**< Module ADC14 is available */
#define __MCU_HAS_AES256__    /**< Module AES256 is available */
#define __MCU_HAS_CAPTIO0__   /**< Module CAPTIO0 is available */
#define __MCU_HAS_CAPTIO1__   /**< Module CAPTIO1 is available */
#define __MCU_HAS_COMP_E0__   /**< Module COMP_E0 is available */
#define __MCU_HAS_COMP_E1__   /**< Module COMP_E1 is available */
#define __MCU_HAS_CRC32__     /**< Module CRC32 is available */
#define __MCU_HAS_CS__        /**< Module CS is available */
#define __MCU_HAS_DIO__       /**< Module DIO is available */
#define __MCU_HAS_DMA__       /**< Module DMA is available */
#define __MCU_HAS_EUSCI_A0__  /**< Module EUSCI_A0 is available */
#define __MCU_HAS_EUSCI_A1__  /**< Module EUSCI_A1 is available */
#define __MCU_HAS_EUSCI_A2__  /**< Module EUSCI_A2 is available */
#define __MCU_HAS_EUSCI_A3__  /**< Module EUSCI_A3 is available */
#define __MCU_HAS_EUSCI_B0__  /**< Module EUSCI_B0 is available */
#define __MCU_HAS_EUSCI_B1__  /**< Module EUSCI_B1 is available */
#define __MCU_HAS_EUSCI_B2__  /**< Module EUSCI_B2 is available */
#define __MCU_HAS_EUSCI_B3__  /**< Module EUSCI_B3 is available */
#define __MCU_HAS_FLCTL__     /**< Module FLCTL is available */
#define __MCU_HAS_PCM__       /**< Module PCM is available */
#define __MCU_HAS_PMAP__      /**< Module PMAP is available */
#define __MCU_HAS_PSS__       /**< Module PSS is available */
#define __MCU_HAS_REF_A__     /**< Module REF_A is available */
#define __MCU_HAS_RSTCTL__    /**< Module RSTCTL is available */
#define __MCU_HAS_RTC_C__     /**< Module RTC_C is available */
#define __MCU_HAS_SYSCTL__    /**< Module SYSCTL is available */
#define __MCU_HAS_TIMER32__   /**< Module TIMER32 is available */
#define __MCU_HAS_TIMER_A0__  /**< Module TIMER_A0 is available */
#define __MCU_HAS_TIMER_A1__  /**< Module TIMER_A1 is available */
#define __MCU_HAS_TIMER_A2__  /**< Module TIMER_A2 is available */
#define __MCU_HAS_TIMER_A3__  /**< Module TIMER_A3 is available */
#define __MCU_HAS_TLV__       /**< Module TLV is available */
#define __MCU_HAS_WDT_A__     /**< Module WDT_A is available */

/* Definitions to show that specific ports are available */

#define __MSP432_HAS_PORTA_R__
#define __MSP432_HAS_PORTB_R__
#define __MSP432_HAS_PORTC_R__
#define __MSP432_HAS_PORTD_R__
#define __MSP432_HAS_PORTE_R__
#define __MSP432_HAS_PORTJ_R__

#define __MSP432_HAS_PORT1_R__
#define __MSP432_HAS_PORT2_R__
#define __MSP432_HAS_PORT3_R__
#define __MSP432_HAS_PORT4_R__
#define __MSP432_HAS_PORT5_R__
#define __MSP432_HAS_PORT6_R__
#define __MSP432_HAS_PORT7_R__
#define __MSP432_HAS_PORT8_R__
#define __MSP432_HAS_PORT9_R__
#define __MSP432_HAS_PORT10_R__

/*@}*/ /* end of group MSP432P401R_CMSIS */

/******************************************************************************
* Device and peripheral memory map                                            *
******************************************************************************/
/** @addtogroup MSP432P401R_MemoryMap MSP432P401R Memory Mapping
  @{
*/
constexpr intptr_t FLASH_BASE   = 0x00000000UL;  /**< Main Flash memory start address */
constexpr intptr_t SRAM_BASE    = 0x20000000UL;  /**< SRAM memory start address */
constexpr intptr_t PERIPH_BASE  = 0x40000000UL;  /**< Peripherals start address */
constexpr intptr_t PERIPH_BASE2 = 0xE0000000UL;  /**< Peripherals start address */

constexpr intptr_t ADC14_BASE        = PERIPH_BASE + 0x00012000UL; /**< Base address of module ADC14 registers */
constexpr intptr_t AES256_BASE       = PERIPH_BASE + 0x00003C00UL; /**< Base address of module AES256 registers */
constexpr intptr_t CAPTIO0_BASE      = PERIPH_BASE + 0x00005400UL; /**< Base address of module CAPTIO0 registers */
constexpr intptr_t CAPTIO1_BASE      = PERIPH_BASE + 0x00005800UL; /**< Base address of module CAPTIO1 registers */
constexpr intptr_t COMP_E0_BASE      = PERIPH_BASE + 0x00003400UL; /**< Base address of module COMP_E0 registers */
constexpr intptr_t COMP_E1_BASE      = PERIPH_BASE + 0x00003800UL; /**< Base address of module COMP_E1 registers */
constexpr intptr_t CRC32_BASE        = PERIPH_BASE + 0x00004000UL; /**< Base address of module CRC32 registers */
constexpr intptr_t CS_BASE           = PERIPH_BASE + 0x00010400UL; /**< Base address of module CS registers */
constexpr intptr_t DIO_BASE          = PERIPH_BASE + 0x00004C00UL; /**< Base address of module DIO registers */
constexpr intptr_t DMA_BASE          = PERIPH_BASE + 0x0000E000UL; /**< Base address of module DMA registers */
constexpr intptr_t EUSCI_A0_BASE     = PERIPH_BASE + 0x00001000UL; /**< Base address of module EUSCI_A0 registers */
constexpr intptr_t EUSCI_A0_SPI_BASE = PERIPH_BASE + 0x00001000UL; /**< Base address of module EUSCI_A0 registers */
constexpr intptr_t EUSCI_A1_BASE     = PERIPH_BASE + 0x00001400UL; /**< Base address of module EUSCI_A1 registers */
constexpr intptr_t EUSCI_A1_SPI_BASE = PERIPH_BASE + 0x00001400UL; /**< Base address of module EUSCI_A1 registers */
constexpr intptr_t EUSCI_A2_BASE     = PERIPH_BASE + 0x00001800UL; /**< Base address of module EUSCI_A2 registers */
constexpr intptr_t EUSCI_A2_SPI_BASE = PERIPH_BASE + 0x00001800UL; /**< Base address of module EUSCI_A2 registers */
constexpr intptr_t EUSCI_A3_BASE     = PERIPH_BASE + 0x00001C00UL; /**< Base address of module EUSCI_A3 registers */
constexpr intptr_t EUSCI_A3_SPI_BASE = PERIPH_BASE + 0x00001C00UL; /**< Base address of module EUSCI_A3 registers */
constexpr intptr_t EUSCI_B0_BASE     = PERIPH_BASE + 0x00002000UL; /**< Base address of module EUSCI_B0 registers */
constexpr intptr_t EUSCI_B0_SPI_BASE = PERIPH_BASE + 0x00002000UL; /**< Base address of module EUSCI_B0 registers */
constexpr intptr_t EUSCI_B1_BASE     = PERIPH_BASE + 0x00002400UL; /**< Base address of module EUSCI_B1 registers */
constexpr intptr_t EUSCI_B1_SPI_BASE = PERIPH_BASE + 0x00002400UL; /**< Base address of module EUSCI_B1 registers */
constexpr intptr_t EUSCI_B2_BASE     = PERIPH_BASE + 0x00002800UL; /**< Base address of module EUSCI_B2 registers */
constexpr intptr_t EUSCI_B2_SPI_BASE = PERIPH_BASE + 0x00002800UL; /**< Base address of module EUSCI_B2 registers */
constexpr intptr_t EUSCI_B3_BASE     = PERIPH_BASE + 0x00002C00UL; /**< Base address of module EUSCI_B3 registers */
constexpr intptr_t EUSCI_B3_SPI_BASE = PERIPH_BASE + 0x00002C00UL; /**< Base address of module EUSCI_B3 registers */
constexpr intptr_t FLCTL_BASE        = PERIPH_BASE + 0x00011000UL; /**< Base address of module FLCTL registers */
constexpr intptr_t PCM_BASE          = PERIPH_BASE + 0x00010000UL; /**< Base address of module PCM registers */
constexpr intptr_t PMAP_BASE         = PERIPH_BASE + 0x00005000UL; /**< Base address of module PMAP registers */
constexpr intptr_t PSS_BASE          = PERIPH_BASE + 0x00010800UL; /**< Base address of module PSS registers */
constexpr intptr_t REF_A_BASE        = PERIPH_BASE + 0x00003000UL; /**< Base address of module REF_A registers */
constexpr intptr_t RSTCTL_BASE       = PERIPH_BASE2+ 0x00042000UL; /**< Base address of module RSTCTL registers */
constexpr intptr_t RTC_C_BASE        = PERIPH_BASE + 0x00004400UL; /**< Base address of module RTC_C registers */
constexpr intptr_t RTC_C_BCD_BASE    = PERIPH_BASE + 0x00004400UL; /**< Base address of module RTC_C registers */
constexpr intptr_t SYSCTL_BASE       = PERIPH_BASE2+ 0x00043000UL; /**< Base address of module SYSCTL registers */
constexpr intptr_t TIMER32_BASE      = PERIPH_BASE + 0x0000C000UL; /**< Base address of module TIMER32 registers */
constexpr intptr_t TIMER_A0_BASE     = PERIPH_BASE + 0x00000000UL; /**< Base address of module TIMER_A0 registers */
constexpr intptr_t TIMER_A1_BASE     = PERIPH_BASE + 0x00000400UL; /**< Base address of module TIMER_A1 registers */
constexpr intptr_t TIMER_A2_BASE     = PERIPH_BASE + 0x00000800UL; /**< Base address of module TIMER_A2 registers */
constexpr intptr_t TIMER_A3_BASE     = PERIPH_BASE + 0x00000C00UL; /**< Base address of module TIMER_A3 registers */
constexpr intptr_t TLV_BASE          = 0x00201000UL;               /**< Base address of module TLV registers */
constexpr intptr_t WDT_A_BASE        = PERIPH_BASE + 0x00004800UL; /**< Base address of module WDT_A registers */
/*@}*/ /* end of group MSP432P401R_MemoryMap */

/******************************************************************************
* Definitions for bit band access                                             *
******************************************************************************/
constexpr intptr_t BITBAND_SRAM_BASE = 0x22000000UL;
constexpr intptr_t BITBAND_PERI_BASE = 0x42000000UL;

/* SRAM allows 32 bit bit band access */
// #define BITBAND_SRAM(x, b)  (*((_IO uint32_t *) (BITBAND_SRAM_BASE +  (((uint32_t)(uint32_t *)&x) - SRAM_BASE  )*32 + b*4)))
/* peripherals with 8 bit or 16 bit register access allow only 8 bit or 16 bit bit band access, so cast to 8 bit always */
// #define BITBAND_PERI(x, b)  (*((_IO  uint8_t *) (BITBAND_PERI_BASE +  (((uint32_t)(uint32_t *)&x) - PERIPH_BASE)*32 + b*4)))

/******************************************************************************
* Peripheral register definitions                                             *
******************************************************************************/
/** @addtogroup MSP432P401R_Peripherals MSP432P401R Peripherals
  MSP432P401R Device Specific Peripheral registers structures
  @{
*/

#if defined ( __CC_ARM )
#pragma anon_unions
#endif

/******************************************************************************
* ADC14 Registers
******************************************************************************/
/** @addtogroup ADC14 MSP432P401R (ADC14)
  @{
*/
typedef struct {
  _IO uint32_t CTL0;         /**< Control 0 Register */
  _IO uint32_t CTL1;         /**< Control 1 Register */
  _IO uint32_t LO0;          /**< Window Comparator Low Threshold 0 Register */
  _IO uint32_t HI0;          /**< Window Comparator High Threshold 0 Register */
  _IO uint32_t LO1;          /**< Window Comparator Low Threshold 1 Register */
  _IO uint32_t HI1;          /**< Window Comparator High Threshold 1 Register */
  _IO uint32_t MCTL[32];     /**< Conversion Memory Control Register */
  _IO uint32_t MEM[32];      /**< Conversion Memory Register */
      uint32_t RESERVED0[9];
  _IO uint32_t IER0;         /**< Interrupt Enable 0 Register */
  _IO uint32_t IER1;         /**< Interrupt Enable 1 Register */
  _I  uint32_t IFGR0;        /**< Interrupt Flag 0 Register */
  _I  uint32_t IFGR1;        /**< Interrupt Flag 1 Register */
  _O  uint32_t CLRIFGR0;     /**< Clear Interrupt Flag 0 Register */
  _IO uint32_t CLRIFGR1;     /**< Clear Interrupt Flag 1 Register */
  _IO uint32_t IV;           /**< Interrupt Vector Register */
} ADC14_Type;

/*@}*/ /* end of group ADC14 */

/******************************************************************************
* AES256 Registers
******************************************************************************/
/** @addtogroup AES256 MSP432P401R (AES256)
  @{
*/
typedef struct {
  _IO uint16_t CTL0;  /**< AES Accelerator Control Register 0 */
  _IO uint16_t CTL1;  /**< AES Accelerator Control Register 1 */
  _IO uint16_t STAT;  /**< AES Accelerator Status Register */
  _O  uint16_t KEY;   /**< AES Accelerator Key Register */
  _O  uint16_t DIN;   /**< AES Accelerator Data In Register */
  _O  uint16_t DOUT;  /**< AES Accelerator Data Out Register */
  _O  uint16_t XDIN;  /**< AES Accelerator XORed Data In Register */
  _O  uint16_t XIN;   /**< AES Accelerator XORed Data In Register */
} AES256_Type;
/*@}*/ /* end of group AES256 */


/******************************************************************************
* CAPTIO Registers
******************************************************************************/
/** @addtogroup CAPTIO MSP432P401R (CAPTIO)
  @{
*/
typedef struct {
      uint16_t RESERVED0[7];
  _IO uint16_t CTL;  /**< Capacitive Touch IO x Control Register */
} CAPTIO_Type;
/*@}*/ /* end of group CAPTIO */

/******************************************************************************
* COMP_E Registers
******************************************************************************/
/** @addtogroup COMP_E MSP432P401R (COMP_E)
  @{
*/
typedef struct {
  _IO uint16_t CTL0;         /**< Comparator Control Register 0 */
  _IO uint16_t CTL1;         /**< Comparator Control Register 1 */
  _IO uint16_t CTL2;         /**< Comparator Control Register 2 */
  _IO uint16_t CTL3;         /**< Comparator Control Register 3 */
      uint16_t RESERVED0[2];
  _IO uint16_t INT;          /**< Comparator Interrupt Control Register */
  _I  uint16_t IV;           /**< Comparator Interrupt Vector Word Register */
} COMP_E_Type;
/*@}*/ /* end of group COMP_E */


/******************************************************************************
* CRC32 Registers
******************************************************************************/
/** @addtogroup CRC32 MSP432P401R (CRC32)
  @{
*/
typedef struct {
  _IO uint16_t DI32;         /**< Data Input for CRC32 Signature Computation */
      uint16_t RESERVED0;
  _IO uint16_t DIRB32;       /**< Data In Reverse for CRC32 Computation */
      uint16_t RESERVED1;
  _IO uint16_t INIRES32_LO;  /**< CRC32 Initialization and Result, lower 16 bits */
  _IO uint16_t INIRES32_HI;  /**< CRC32 Initialization and Result, upper 16 bits */
  _IO uint16_t RESR32_LO;    /**< CRC32 Result Reverse, lower 16 bits */
  _IO uint16_t RESR32_HI;    /**< CRC32 Result Reverse, Upper 16 bits */
  _IO uint16_t DI16;         /**< Data Input for CRC16 computation */
      uint16_t RESERVED2;
  _IO uint16_t DIRB16;       /**< CRC16 Data In Reverse */
      uint16_t RESERVED3;
  _IO uint16_t INIRES16;     /**< CRC16 Initialization and Result register */
      uint16_t RESERVED4[2];
  _IO uint16_t RESR16;       /**< CRC16 Result Reverse */
} CRC32_Type;

/*@}*/ /* end of group CRC32 */


/******************************************************************************
* CS Registers
******************************************************************************/
/** @addtogroup CS MSP432P401R (CS)
  @{
*/
typedef struct {
  _IO uint32_t KEY;          /**< Key Register */
  _IO uint32_t CTL0;         /**< Control 0 Register */
  _IO uint32_t CTL1;         /**< Control 1 Register */
  _IO uint32_t CTL2;         /**< Control 2 Register */
  _IO uint32_t CTL3;         /**< Control 3 Register */
  _IO uint32_t CTL4;         /**< Control 4 Register */
  _IO uint32_t CTL5;         /**< Control 5 Register */
  _IO uint32_t CTL6;         /**< Control 6 Register */
  _IO uint32_t CTL7;         /**< Control 7 Register */
      uint32_t RESERVED0[3];
  _IO uint32_t CLKEN;        /**< Clock Enable Register */
  _I  uint32_t STAT;         /**< Status Register */
      uint32_t RESERVED1[2];
  _IO uint32_t IE;           /**< Interrupt Enable Register */
      uint32_t RESERVED2;
  _I  uint32_t IFG;          /**< Interrupt Flag Register */
      uint32_t RESERVED3;
  _O  uint32_t CLRIFG;       /**< Clear Interrupt Flag Register */
      uint32_t RESERVED4;
  _O  uint32_t SETIFG;       /**< Set Interrupt Flag Register */
      uint32_t RESERVED5;
  _IO uint32_t DCOERCAL0;    /**< DCO External Resistor Cailbration 0 Register */
  _IO uint32_t DCOERCAL1;    /**< DCO External Resistor Calibration 1 Register */
} CS_Type;
/*@}*/ /* end of group CS */

/******************************************************************************
* DIO Registers
******************************************************************************/
/** @addtogroup DIO MSP432P401R (DIO)
  @{
*/
typedef struct {
  union {
    _I uint16_t IN;    /**< Port Pair Input */
    struct {
      _I uint8_t IN_L; /**< Low Port Input */
      _I uint8_t IN_H; /**< High Port Input */
    };
  };
  union {
    _IO uint16_t OUT;    /**< Port Pair Output */
    struct {
      _IO uint8_t OUT_L; /**< Low Port Output */
      _IO uint8_t OUT_H; /**< High Port Output */
    };
  };
  union {
    _IO uint16_t DIR;    /**< Port Pair Direction */
    struct {
      _IO uint8_t DIR_L; /**< Low Port Direction */
      _IO uint8_t DIR_H; /**< High Port Direction */
    };
  };
  union {
    _IO uint16_t REN;    /**< Port Pair Resistor Enable */
    struct {
      _IO uint8_t REN_L; /**< Low Port Resistor Enable */
      _IO uint8_t REN_H; /**< High Port Resistor Enable */
    };
  };
  union {
    _IO uint16_t DS;    /**< Port Pair Drive Strength */
    struct {
      _IO uint8_t DS_L; /**< Low Port Drive Strength */
      _IO uint8_t DS_H; /**< High Port Drive Strength */
    };
  };
  union {
    _IO uint16_t SEL0;    /**< Port Pair Select 0 */
    struct {
      _IO uint8_t SEL0_L; /**< Low Port Select 0 */
      _IO uint8_t SEL0_H; /**< High Port Select 0 */
    };
  };
  union {
    _IO uint16_t SEL1;    /**< Port Pair Select 1 */
    struct {
      _IO uint8_t SEL1_L; /**< Low Port Select 1 */
      _IO uint8_t SEL1_H; /**< High Port Select 1 */
    };
  };
  _I  uint16_t IV_L;      /**< Low Port Interrupt Vector Value */
  uint16_t  RESERVED0[3];
  union {
    _IO uint16_t SELC;    /**< Port Pair Complement Select */
    struct {
      _IO uint8_t SELC_L; /**< Low Port Complement Select */
      _IO uint8_t SELC_H; /**< High Port Complement Select */
    };
  };
  union {
    _IO uint16_t IES;    /**< Port Pair Interrupt Edge Select */
    struct {
      _IO uint8_t IES_L; /**< Low Port Interrupt Edge Select */
      _IO uint8_t IES_H; /**< High Port Interrupt Edge Select */
    };
  };
  union {
    _IO uint16_t IE;    /**< Port Pair Interrupt Enable */
    struct {
      _IO uint8_t IE_L; /**< Low Port Interrupt Enable */
      _IO uint8_t IE_H; /**< High Port Interrupt Enable */
    };
  };
  union {
    _IO uint16_t IFG;    /**< Port Pair Interrupt Flag */
    struct {
      _IO uint8_t IFG_L; /**< Low Port Interrupt Flag */
      _IO uint8_t IFG_H; /**< High Port Interrupt Flag */
    };
  };
  _I uint16_t IV_H;      /**< High Port Interrupt Vector Value */
} DIO_PORT_Interruptable_Type;

typedef struct {
  union {
    _I uint16_t IN;    /**< Port Pair Input */
    struct {
      _I uint8_t IN_L; /**< Low Port Input */
      _I uint8_t IN_H; /**< High Port Input */
    };
  };
  union {
    _IO uint16_t OUT;    /**< Port Pair Output */
    struct {
      _IO uint8_t OUT_L; /**< Low Port Output */
      _IO uint8_t OUT_H; /**< High Port Output */
    };
  };
  union {
    _IO uint16_t DIR;    /**< Port Pair Direction */
    struct {
      _IO uint8_t DIR_L; /**< Low Port Direction */
      _IO uint8_t DIR_H; /**< High Port Direction */
    };
  };
  union {
    _IO uint16_t REN;    /**< Port Pair Resistor Enable */
    struct {
      _IO uint8_t REN_L; /**< Low Port Resistor Enable */
      _IO uint8_t REN_H; /**< High Port Resistor Enable */
    };
  };
  union {
    _IO uint16_t DS;    /**< Port Pair Drive Strength */
    struct {
      _IO uint8_t DS_L; /**< Low Port Drive Strength */
      _IO uint8_t DS_H; /**< High Port Drive Strength */
    };
  };
  union {
    _IO uint16_t SEL0;    /**< Port Pair Select 0 */
    struct {
      _IO uint8_t SEL0_L; /**< Low Port Select 0 */
      _IO uint8_t SEL0_H; /**< High Port Select 0 */
    };
  };
  union {
    _IO uint16_t SEL1;    /**< Port Pair Select 1 */
    struct {
      _IO uint8_t SEL1_L; /**< Low Port Select 1 */
      _IO uint8_t SEL1_H; /**< High Port Select 1 */
    };
  };
  uint16_t  RESERVED0[4];
  union {
    _IO uint16_t SELC;    /**< Port Pair Complement Select */
    struct {
      _IO uint8_t SELC_L; /**< Low Port Complement Select */
      _IO uint8_t SELC_H; /**< High Port Complement Select */
    };
  };
} DIO_PORT_Not_Interruptable_Type;


typedef struct {
  _I uint8_t IN;       /**< Port Input */
  uint8_t RESERVED0;
  _IO uint8_t OUT;     /**< Port Output */
  uint8_t RESERVED1;
  _IO uint8_t DIR;     /**< Port Direction */
  uint8_t RESERVED2;
  _IO uint8_t REN;     /**< Port Resistor Enable */
  uint8_t RESERVED3;
  _IO uint8_t DS;      /**< Port Drive Strength */
  uint8_t RESERVED4;
  _IO uint8_t SEL0;    /**< Port Select 0 */
  uint8_t RESERVED5;
  _IO uint8_t SEL1;    /**< Port Select 1 */
  uint8_t RESERVED6;
  _I  uint16_t IV;     /**< Port Interrupt Vector Value */
  uint8_t RESERVED7[6];
  _IO uint8_t SELC;    /**< Port Complement Select */
  uint8_t RESERVED8;
  _IO uint8_t IES;     /**< Port Interrupt Edge Select */
  uint8_t RESERVED9;
  _IO uint8_t IE;      /**< Port Interrupt Enable */
  uint8_t RESERVED10;
  _IO uint8_t IFG;     /**< Port Interrupt Flag */
} DIO_PORT_Odd_Interruptable_Type;

typedef struct {
  uint8_t RESERVED0;
  _I uint8_t IN;       /**< Port Input */
  uint8_t RESERVED1;
  _IO uint8_t OUT;     /**< Port Output */
  uint8_t RESERVED2;
  _IO uint8_t DIR;     /**< Port Direction */
  uint8_t RESERVED3;
  _IO uint8_t REN;     /**< Port Resistor Enable */
  uint8_t RESERVED4;
  _IO uint8_t DS;      /**< Port Drive Strength */
  uint8_t RESERVED5;
  _IO uint8_t SEL0;    /**< Port Select 0 */
  uint8_t RESERVED6;
  _IO uint8_t SEL1;    /**< Port Select 1 */
  uint8_t RESERVED7[9];
  _IO uint8_t SELC;    /**< Port Complement Select */
  uint8_t RESERVED8;
  _IO uint8_t IES;     /**< Port Interrupt Edge Select */
  uint8_t RESERVED9;
  _IO uint8_t IE;      /**< Port Interrupt Enable */
  uint8_t RESERVED10;
  _IO uint8_t IFG;     /**< Port Interrupt Flag */
  _I uint16_t IV;      /**< Port Interrupt Vector Value */
} DIO_PORT_Even_Interruptable_Type;

/*@}*/ /* end of group MSP432P401R_DIO */


/******************************************************************************
* DMA Registers
******************************************************************************/
/** @addtogroup DMA MSP432P401R (DMA)
  @{
*/
typedef struct {
  _I  uint32_t DEVICE_CFG;                                                      /**< Device Configuration Status */
  _IO uint32_t SW_CHTRIG;                                                       /**< Software Channel Trigger Register */
       uint32_t RESERVED0[2];
  _IO uint32_t CH_SRCCFG[32];                                                   /**< Channel n Source Configuration Register */
       uint32_t RESERVED1[28];
  _IO uint32_t INT1_SRCCFG;                                                     /**< Interrupt 1 Source Channel Configuration */
  _IO uint32_t INT2_SRCCFG;                                                     /**< Interrupt 2 Source Channel Configuration Register */
  _IO uint32_t INT3_SRCCFG;                                                     /**< Interrupt 3 Source Channel Configuration Register */
       uint32_t RESERVED2;
  _I  uint32_t INT0_SRCFLG;                                                     /**< Interrupt 0 Source Channel Flag Register */
  _O  uint32_t INT0_CLRFLG;                                                     /**< Interrupt 0 Source Channel Clear Flag Register */
} DMA_Channel_Type;

typedef struct {
  _I  uint32_t STAT;                                                            /**< Status Register */
  _O  uint32_t CFG;                                                             /**< Configuration Register */
  _IO uint32_t CTLBASE;                                                         /**< Channel Control Data Base Pointer Register */
  _I  uint32_t ATLBASE;                                                         /**< Channel Alternate Control Data Base Pointer Register */
  _I  uint32_t WAITSTAT;                                                        /**< Channel Wait on Request Status Register */
  _O  uint32_t SWREQ;                                                           /**< Channel Software Request Register */
  _IO uint32_t USEBURSTSET;                                                     /**< Channel Useburst Set Register */
  _O  uint32_t USEBURSTCLR;                                                     /**< Channel Useburst Clear Register */
  _IO uint32_t REQMASKSET;                                                      /**< Channel Request Mask Set Register */
  _O  uint32_t REQMASKCLR;                                                      /**< Channel Request Mask Clear Register */
  _IO uint32_t ENASET;                                                          /**< Channel Enable Set Register */
  _O  uint32_t ENACLR;                                                          /**< Channel Enable Clear Register */
  _IO uint32_t ALTSET;                                                          /**< Channel Primary-Alternate Set Register */
  _O  uint32_t ALTCLR;                                                          /**< Channel Primary-Alternate Clear Register */
  _IO uint32_t PRIOSET;                                                         /**< Channel Priority Set Register */
  _O  uint32_t PRIOCLR;                                                         /**< Channel Priority Clear Register */
       uint32_t RESERVED4[3];
  _IO uint32_t ERRCLR;                                                          /**< Bus Error Clear Register */
} DMA_Control_Type;

/*@}*/ /* end of group DMA */


/******************************************************************************
* EUSCI_A Registers
******************************************************************************/
/** @addtogroup EUSCI_A MSP432P401R (EUSCI_A)
  @{
*/
typedef struct {
  _IO uint16_t CTLW0;                                                           /**< eUSCI_Ax Control Word Register 0 */
  _IO uint16_t CTLW1;                                                           /**< eUSCI_Ax Control Word Register 1 */
       uint16_t RESERVED0;
  _IO uint16_t BRW;                                                             /**< eUSCI_Ax Baud Rate Control Word Register */
  _IO uint16_t MCTLW;                                                           /**< eUSCI_Ax Modulation Control Word Register */
  _IO uint16_t STATW;                                                           /**< eUSCI_Ax Status Register */
  _I  uint16_t RXBUF;                                                           /**< eUSCI_Ax Receive Buffer Register */
  _IO uint16_t TXBUF;                                                           /**< eUSCI_Ax Transmit Buffer Register */
  _IO uint16_t ABCTL;                                                           /**< eUSCI_Ax Auto Baud Rate Control Register */
  _IO uint16_t IRCTL;                                                           /**< eUSCI_Ax IrDA Control Word Register */
       uint16_t RESERVED1[3];
  _IO uint16_t IE;                                                              /**< eUSCI_Ax Interrupt Enable Register */
  _IO uint16_t IFG;                                                             /**< eUSCI_Ax Interrupt Flag Register */
  _I  uint16_t IV;                                                              /**< eUSCI_Ax Interrupt Vector Register */
} EUSCI_A_Type;

/*@}*/ /* end of group EUSCI_A */

/** @addtogroup EUSCI_A_SPI MSP432P401R (EUSCI_A_SPI)
  @{
*/
typedef struct {
  _IO uint16_t CTLW0;                                                           /**< eUSCI_Ax Control Word Register 0 */
       uint16_t RESERVED0[2];
  _IO uint16_t BRW;                                                             /**< eUSCI_Ax Bit Rate Control Register 1 */
       uint16_t RESERVED1;
  _IO uint16_t STATW;
  _I  uint16_t RXBUF;                                                           /**< eUSCI_Ax Receive Buffer Register */
  _IO uint16_t TXBUF;                                                           /**< eUSCI_Ax Transmit Buffer Register */
       uint16_t RESERVED2[5];
  _IO uint16_t IE;                                                              /**< eUSCI_Ax Interrupt Enable Register */
  _IO uint16_t IFG;                                                             /**< eUSCI_Ax Interrupt Flag Register */
  _I  uint16_t IV;                                                              /**< eUSCI_Ax Interrupt Vector Register */
} EUSCI_A_SPI_Type;

/*@}*/ /* end of group EUSCI_A_SPI */


/******************************************************************************
* EUSCI_B Registers
******************************************************************************/
/** @addtogroup EUSCI_B MSP432P401R (EUSCI_B)
  @{
*/
typedef struct {
  _IO uint16_t CTLW0;                                                           /**< eUSCI_Bx Control Word Register 0 */
  _IO uint16_t CTLW1;                                                           /**< eUSCI_Bx Control Word Register 1 */
       uint16_t RESERVED0;
  _IO uint16_t BRW;                                                             /**< eUSCI_Bx Baud Rate Control Word Register */
  _IO uint16_t STATW;                                                           /**< eUSCI_Bx Status Register */
  _IO uint16_t TBCNT;                                                           /**< eUSCI_Bx Byte Counter Threshold Register */
  _I  uint16_t RXBUF;                                                           /**< eUSCI_Bx Receive Buffer Register */
  _IO uint16_t TXBUF;                                                           /**< eUSCI_Bx Transmit Buffer Register */
       uint16_t RESERVED1[2];
  _IO uint16_t I2COA0;                                                          /**< eUSCI_Bx I2C Own Address 0 Register */
  _IO uint16_t I2COA1;                                                          /**< eUSCI_Bx I2C Own Address 1 Register */
  _IO uint16_t I2COA2;                                                          /**< eUSCI_Bx I2C Own Address 2 Register */
  _IO uint16_t I2COA3;                                                          /**< eUSCI_Bx I2C Own Address 3 Register */
  _I  uint16_t ADDRX;                                                           /**< eUSCI_Bx I2C Received Address Register */
  _IO uint16_t ADDMASK;                                                         /**< eUSCI_Bx I2C Address Mask Register */
  _IO uint16_t I2CSA;                                                           /**< eUSCI_Bx I2C Slave Address Register */
       uint16_t RESERVED2[4];
  _IO uint16_t IE;                                                              /**< eUSCI_Bx Interrupt Enable Register */
  _IO uint16_t IFG;                                                             /**< eUSCI_Bx Interrupt Flag Register */
  _I  uint16_t IV;                                                              /**< eUSCI_Bx Interrupt Vector Register */
} EUSCI_B_Type;

/*@}*/ /* end of group EUSCI_B */

/** @addtogroup EUSCI_B_SPI MSP432P401R (EUSCI_B_SPI)
  @{
*/
typedef struct {
  _IO uint16_t CTLW0;                                                           /**< eUSCI_Bx Control Word Register 0 */
       uint16_t RESERVED0[2];
  _IO uint16_t BRW;                                                             /**< eUSCI_Bx Bit Rate Control Register 1 */
  _IO uint16_t STATW;
       uint16_t RESERVED1;
  _I  uint16_t RXBUF;                                                           /**< eUSCI_Bx Receive Buffer Register */
  _IO uint16_t TXBUF;                                                           /**< eUSCI_Bx Transmit Buffer Register */
       uint16_t RESERVED2[13];
  _IO uint16_t IE;                                                              /**< eUSCI_Bx Interrupt Enable Register */
  _IO uint16_t IFG;                                                             /**< eUSCI_Bx Interrupt Flag Register */
  _I  uint16_t IV;                                                              /**< eUSCI_Bx Interrupt Vector Register */
} EUSCI_B_SPI_Type;

/*@}*/ /* end of group EUSCI_B_SPI */


/******************************************************************************
* FLCTL Registers
******************************************************************************/
/** @addtogroup FLCTL MSP432P401R (FLCTL)
  @{
*/
typedef struct {
  _I  uint32_t POWER_STAT;                                                      /**< Power Status Register */
       uint32_t RESERVED0[3];
  _IO uint32_t BANK0_RDCTL;                                                     /**< Bank0 Read Control Register */
  _IO uint32_t BANK1_RDCTL;                                                     /**< Bank1 Read Control Register */
       uint32_t RESERVED1[2];
  _IO uint32_t RDBRST_CTLSTAT;                                                  /**< Read Burst/Compare Control and Status Register */
  _IO uint32_t RDBRST_STARTADDR;                                                /**< Read Burst/Compare Start Address Register */
  _IO uint32_t RDBRST_LEN;                                                      /**< Read Burst/Compare Length Register */
       uint32_t RESERVED2[4];
  _IO uint32_t RDBRST_FAILADDR;                                                 /**< Read Burst/Compare Fail Address Register */
  _IO uint32_t RDBRST_FAILCNT;                                                  /**< Read Burst/Compare Fail Count Register */
       uint32_t RESERVED3[3];
  _IO uint32_t PRG_CTLSTAT;                                                     /**< Program Control and Status Register */
  _IO uint32_t PRGBRST_CTLSTAT;                                                 /**< Program Burst Control and Status Register */
  _IO uint32_t PRGBRST_STARTADDR;                                               /**< Program Burst Start Address Register */
       uint32_t RESERVED4;
  _IO uint32_t PRGBRST_DATA0_0;                                                 /**< Program Burst Data0 Register0 */
  _IO uint32_t PRGBRST_DATA0_1;                                                 /**< Program Burst Data0 Register1 */
  _IO uint32_t PRGBRST_DATA0_2;                                                 /**< Program Burst Data0 Register2 */
  _IO uint32_t PRGBRST_DATA0_3;                                                 /**< Program Burst Data0 Register3 */
  _IO uint32_t PRGBRST_DATA1_0;                                                 /**< Program Burst Data1 Register0 */
  _IO uint32_t PRGBRST_DATA1_1;                                                 /**< Program Burst Data1 Register1 */
  _IO uint32_t PRGBRST_DATA1_2;                                                 /**< Program Burst Data1 Register2 */
  _IO uint32_t PRGBRST_DATA1_3;                                                 /**< Program Burst Data1 Register3 */
  _IO uint32_t PRGBRST_DATA2_0;                                                 /**< Program Burst Data2 Register0 */
  _IO uint32_t PRGBRST_DATA2_1;                                                 /**< Program Burst Data2 Register1 */
  _IO uint32_t PRGBRST_DATA2_2;                                                 /**< Program Burst Data2 Register2 */
  _IO uint32_t PRGBRST_DATA2_3;                                                 /**< Program Burst Data2 Register3 */
  _IO uint32_t PRGBRST_DATA3_0;                                                 /**< Program Burst Data3 Register0 */
  _IO uint32_t PRGBRST_DATA3_1;                                                 /**< Program Burst Data3 Register1 */
  _IO uint32_t PRGBRST_DATA3_2;                                                 /**< Program Burst Data3 Register2 */
  _IO uint32_t PRGBRST_DATA3_3;                                                 /**< Program Burst Data3 Register3 */
  _IO uint32_t ERASE_CTLSTAT;                                                   /**< Erase Control and Status Register */
  _IO uint32_t ERASE_SECTADDR;                                                  /**< Erase Sector Address Register */
       uint32_t RESERVED5[2];
  _IO uint32_t BANK0_INFO_WEPROT;                                               /**< Information Memory Bank0 Write/Erase Protection Register */
  _IO uint32_t BANK0_MAIN_WEPROT;                                               /**< Main Memory Bank0 Write/Erase Protection Register */
       uint32_t RESERVED6[2];
  _IO uint32_t BANK1_INFO_WEPROT;                                               /**< Information Memory Bank1 Write/Erase Protection Register */
  _IO uint32_t BANK1_MAIN_WEPROT;                                               /**< Main Memory Bank1 Write/Erase Protection Register */
       uint32_t RESERVED7[2];
  _IO uint32_t BMRK_CTLSTAT;                                                    /**< Benchmark Control and Status Register */
  _IO uint32_t BMRK_IFETCH;                                                     /**< Benchmark Instruction Fetch Count Register */
  _IO uint32_t BMRK_DREAD;                                                      /**< Benchmark Data Read Count Register */
  _IO uint32_t BMRK_CMP;                                                        /**< Benchmark Count Compare Register */
       uint32_t RESERVED8[4];
  _IO uint32_t IFG;                                                             /**< Interrupt Flag Register */
  _IO uint32_t IE;                                                              /**< Interrupt Enable Register */
  _IO uint32_t CLRIFG;                                                          /**< Clear Interrupt Flag Register */
  _IO uint32_t SETIFG;                                                          /**< Set Interrupt Flag Register */
  _I  uint32_t READ_TIMCTL;                                                     /**< Read Timing Control Register */
  _I  uint32_t READMARGIN_TIMCTL;                                               /**< Read Margin Timing Control Register */
  _I  uint32_t PRGVER_TIMCTL;                                                   /**< Program Verify Timing Control Register */
  _I  uint32_t ERSVER_TIMCTL;                                                   /**< Erase Verify Timing Control Register */
  _I  uint32_t LKGVER_TIMCTL;                                                   /**< Leakage Verify Timing Control Register */
  _I  uint32_t PROGRAM_TIMCTL;                                                  /**< Program Timing Control Register */
  _I  uint32_t ERASE_TIMCTL;                                                    /**< Erase Timing Control Register */
  _I  uint32_t MASSERASE_TIMCTL;                                                /**< Mass Erase Timing Control Register */
  _I  uint32_t BURSTPRG_TIMCTL;                                                 /**< Burst Program Timing Control Register */
} FLCTL_Type;

/*@}*/ /* end of group FLCTL */


/******************************************************************************
* PCM Registers
******************************************************************************/
/** @addtogroup PCM MSP432P401R (PCM)
  @{
*/
typedef struct {
  _IO uint32_t CTL0;                                                            /**< Control 0 Register */
  _IO uint32_t CTL1;                                                            /**< Control 1 Register */
  _IO uint32_t IE;                                                              /**< Interrupt Enable Register */
  _I  uint32_t IFG;                                                             /**< Interrupt Flag Register */
  _O  uint32_t CLRIFG;                                                          /**< Clear Interrupt Flag Register */
} PCM_Type;

/*@}*/ /* end of group PCM */

/******************************************************************************
* PMAP Registers
******************************************************************************/
/** @addtogroup PMAP MSP432P401R (PMAP)
  @{
*/
typedef struct {
  _IO uint16_t KEYID;
  _IO uint16_t CTL;
} PMAP_COMMON_Type;

typedef struct {
  union {
    _IO uint16_t PMAP_REGISTER[4];
    struct {
      _IO uint8_t PMAP_REGISTER0;
      _IO uint8_t PMAP_REGISTER1;
      _IO uint8_t PMAP_REGISTER2;
      _IO uint8_t PMAP_REGISTER3;
      _IO uint8_t PMAP_REGISTER4;
      _IO uint8_t PMAP_REGISTER5;
      _IO uint8_t PMAP_REGISTER6;
      _IO uint8_t PMAP_REGISTER7;
    };
  };
} PMAP_REGISTER_Type;

/*@}*/ /* end of group PMAP */


/******************************************************************************
* PSS Registers
******************************************************************************/
/** @addtogroup PSS MSP432P401R (PSS)
  @{
*/
typedef struct {
  _IO uint32_t KEY;                                                             /**< Key Register */
  _IO uint32_t CTL0;                                                            /**< Control 0 Register */
       uint32_t RESERVED0[11];
  _IO uint32_t IE;                                                              /**< Interrupt Enable Register */
  _I  uint32_t IFG;                                                             /**< Interrupt Flag Register */
  _IO uint32_t CLRIFG;                                                          /**< Clear Interrupt Flag Register */
} PSS_Type;

/*@}*/ /* end of group PSS */


/******************************************************************************
* REF_A Registers
******************************************************************************/
/** @addtogroup REF_A MSP432P401R (REF_A)
  @{
*/
typedef struct {
  _IO uint16_t CTL0;                                                            /**< REF Control Register 0 */
} REF_A_Type;

/*@}*/ /* end of group REF_A */


/******************************************************************************
* RSTCTL Registers
******************************************************************************/
/** @addtogroup RSTCTL MSP432P401R (RSTCTL)
  @{
*/
typedef struct {
  _IO uint32_t RESET_REQ;                                                       /**< Reset Request Register */
  _I  uint32_t HARDRESET_STAT;                                                  /**< Hard Reset Status Register */
  _IO uint32_t HARDRESET_CLR;                                                   /**< Hard Reset Status Clear Register */
  _IO uint32_t HARDRESET_SET;                                                   /**< Hard Reset Status Set Register */
  _I  uint32_t SOFTRESET_STAT;                                                  /**< Soft Reset Status Register */
  _IO uint32_t SOFTRESET_CLR;                                                   /**< Soft Reset Status Clear Register */
  _IO uint32_t SOFTRESET_SET;                                                   /**< Soft Reset Status Set Register */
       uint32_t RESERVED0[57];
  _I  uint32_t PSSRESET_STAT;                                                   /**< PSS Reset Status Register */
  _IO uint32_t PSSRESET_CLR;                                                    /**< PSS Reset Status Clear Register */
  _I  uint32_t PCMRESET_STAT;                                                   /**< PCM Reset Status Register */
  _IO uint32_t PCMRESET_CLR;                                                    /**< PCM Reset Status Clear Register */
  _I  uint32_t PINRESET_STAT;                                                   /**< Pin Reset Status Register */
  _IO uint32_t PINRESET_CLR;                                                    /**< Pin Reset Status Clear Register */
  _I  uint32_t REBOOTRESET_STAT;                                                /**< Reboot Reset Status Register */
  _IO uint32_t REBOOTRESET_CLR;                                                 /**< Reboot Reset Status Clear Register */
  _I  uint32_t CSRESET_STAT;                                                    /**< CS Reset Status Register */
  _IO uint32_t CSRESET_CLR;                                                     /**< CS Reset Status Clear Register */
} RSTCTL_Type;

/*@}*/ /* end of group RSTCTL */


/******************************************************************************
* RTC_C Registers
******************************************************************************/
/** @addtogroup RTC_C MSP432P401R (RTC_C)
  @{
*/
typedef struct {
  _IO uint16_t CTL0;                                                            /**< RTCCTL0 Register */
  _IO uint16_t CTL13;                                                           /**< RTCCTL13 Register */
  _IO uint16_t OCAL;                                                            /**< RTCOCAL Register */
  _IO uint16_t TCMP;                                                            /**< RTCTCMP Register */
  _IO uint16_t PS0CTL;                                                          /**< Real-Time Clock Prescale Timer 0 Control Register */
  _IO uint16_t PS1CTL;                                                          /**< Real-Time Clock Prescale Timer 1 Control Register */
  _IO uint16_t PS;                                                              /**< Real-Time Clock Prescale Timer Counter Register */
  _I  uint16_t IV;                                                              /**< Real-Time Clock Interrupt Vector Register */
  _IO uint16_t TIM0;                                                            /**< RTCTIM0 Register ? Hexadecimal Format */
  _IO uint16_t TIM1;                                                            /**< Real-Time Clock Hour, Day of Week */
  _IO uint16_t DATE;                                                            /**< RTCDATE - Hexadecimal Format */
  _IO uint16_t YEAR;                                                            /**< RTCYEAR Register ? Hexadecimal Format */
  _IO uint16_t AMINHR;                                                          /**< RTCMINHR - Hexadecimal Format */
  _IO uint16_t ADOWDAY;                                                         /**< RTCADOWDAY - Hexadecimal Format */
  _IO uint16_t BIN2BCD;                                                         /**< Binary-to-BCD Conversion Register */
  _IO uint16_t BCD2BIN;                                                         /**< BCD-to-Binary Conversion Register */
} RTC_C_Type;

/*@}*/ /* end of group RTC_C */

/** @addtogroup RTC_C_BCD MSP432P401R (RTC_C_BCD)
  @{
*/
typedef struct {
       uint16_t RESERVED0[8];
  _IO uint16_t TIM0;                                                            /**< RTCTIM0 Register ? BCD Format */
  _IO uint16_t TIM1;                                                            /**< RTCTIM1 Register ? BCD Format */
  _IO uint16_t DATE;                                                            /**< Real-Time Clock Date - BCD Format */
  _IO uint16_t YEAR;                                                            /**< RTCYEAR Register ? BCD Format */
  _IO uint16_t AMINHR;                                                          /**< RTCMINHR - BCD Format */
  _IO uint16_t ADOWDAY;                                                         /**< RTCADOWDAY - BCD Format */
} RTC_C_BCD_Type;

/*@}*/ /* end of group RTC_C_BCD */


/******************************************************************************
* SYSCTL Registers
******************************************************************************/
/** @addtogroup SYSCTL MSP432P401R (SYSCTL)
  @{
*/
typedef struct {
  _IO uint32_t REBOOT_CTL;                                                      /**< Reboot Control Register */
  _IO uint32_t NMI_CTLSTAT;                                                     /**< NMI Control and Status Register */
  _IO uint32_t WDTRESET_CTL;                                                    /**< Watchdog Reset Control Register */
  _IO uint32_t PERIHALT_CTL;                                                    /**< Peripheral Halt Control Register */
  _I  uint32_t SRAM_SIZE;                                                       /**< SRAM Size Register */
  _IO uint32_t SRAM_BANKEN;                                                     /**< SRAM Bank Enable Register */
  _IO uint32_t SRAM_BANKRET;                                                    /**< SRAM Bank Retention Control Register */
       uint32_t RESERVED0;
  _I  uint32_t FLASH_SIZE;                                                      /**< Flash Size Register */
       uint32_t RESERVED1[3];
  _IO uint32_t DIO_GLTFLT_CTL;                                                  /**< Digital I/O Glitch Filter Control Register */
       uint32_t RESERVED2[3];
  _IO uint32_t SECDATA_UNLOCK;                                                  /**< IP Protected Secure Zone Data Access Unlock Register */
       uint32_t RESERVED3[175];
  _IO uint32_t CSYS_MASTER_UNLOCK;                                              /**< Master Unlock Register */
  _IO uint32_t BOOT_CTL;                                                        /**< Boot Control Register */
       uint32_t RESERVED4[2];
  _IO uint32_t SEC_CTL;                                                         /**< Security Control Register */
       uint32_t RESERVED5[3];
  _IO uint32_t SEC_STARTADDR0;                                                  /**< Security Zone 0 Start Address Register */
  _IO uint32_t SEC_STARTADDR1;                                                  /**< Security Zone 1 Start Address Register */
  _IO uint32_t SEC_STARTADDR2;                                                  /**< Security Zone 2 Start Address Register */
  _IO uint32_t SEC_STARTADDR3;                                                  /**< Security Zone 3 Start Address Register */
  _IO uint32_t SEC_SIZE0;                                                       /**< Security Zone 0 Size Register */
  _IO uint32_t SEC_SIZE1;                                                       /**< Security Zone 1 Size Register */
  _IO uint32_t SEC_SIZE2;                                                       /**< Security Zone 2 Size Register */
  _IO uint32_t SEC_SIZE3;                                                       /**< Security Zone 3 Size Register */
  _IO uint32_t ETW_CTL;                                                         /**< ETW Control Register */
  _IO uint32_t FLASH_SIZECFG;                                                   /**< Flash Size Configuration Register */
  _IO uint32_t SRAM_SIZECFG;                                                    /**< SRAM Size Configuration Register */
  _IO uint32_t SRAM_NUMBANK;                                                    /**< SRAM NUM BANK Configuration Register */
  _IO uint32_t TIMER_CFG;                                                       /**< Timer Configuration Register */
  _IO uint32_t EUSCI_CFG;                                                       /**< eUSCI Configuration Register */
  _IO uint32_t ADC_CFG;                                                         /**< ADC Configuration Register */
  _IO uint32_t XTAL_CFG;                                                        /**< Crystal Oscillator Configuration Register */
  _IO uint32_t BOC_CFG;                                                         /**< Bond Out Configuration Register */
} SYSCTL_Type;

typedef struct {
  _IO uint32_t MASTER_UNLOCK;                                                   /**< Master Unlock Register */
  _IO uint32_t BOOTOVER_REQ[2];                                                 /**< Boot Override Request Register */
  _IO uint32_t BOOTOVER_ACK;                                                    /**< Boot Override Acknowledge Register */
  _IO uint32_t RESET_REQ;                                                       /**< Reset Request Register */
  _IO uint32_t RESET_STATOVER;                                                  /**< Reset Status and Override Register */
       uint32_t RESERVED7[2];
  _I  uint32_t SYSTEM_STAT;                                                     /**< System Status Register */
} SYSCTL_Boot_Type;

/*@}*/ /* end of group SYSCTL */


/******************************************************************************
* Timer32 Registers
******************************************************************************/
/** @addtogroup Timer32 MSP432P401R (Timer32)
  @{
*/
typedef struct {
  _IO uint32_t LOAD;                                                            /**< Timer 1 Load Register */
  _I  uint32_t VALUE;                                                           /**< Timer 1 Current Value Register */
  _IO uint32_t CONTROL;                                                         /**< Timer 1 Timer Control Register */
  _O  uint32_t INTCLR;                                                          /**< Timer 1 Interrupt Clear Register */
  _I  uint32_t RIS;                                                             /**< Timer 1 Raw Interrupt Status Register */
  _I  uint32_t MIS;                                                             /**< Timer 1 Interrupt Status Register */
  _IO uint32_t BGLOAD;                                                          /**< Timer 1 Background Load Register */
} Timer32_Type;

/*@}*/ /* end of group Timer32 */


/******************************************************************************
* Timer_A Registers
******************************************************************************/
/** @addtogroup Timer_A MSP432P401R (Timer_A)
  @{
*/
typedef struct {
  _IO uint16_t CTL;                                                             /**< TimerAx Control Register */
  _IO uint16_t CCTL[7];                                                         /**< Timer_A Capture/Compare Control Register */
  _IO uint16_t R;                                                               /**< TimerA register */
  _IO uint16_t CCR[7];                                                          /**< Timer_A Capture/Compare  Register */
  _IO uint16_t EX0;                                                             /**< TimerAx Expansion 0 Register */
       uint16_t RESERVED0[6];
  _I  uint16_t IV;                                                              /**< TimerAx Interrupt Vector Register */
} Timer_A_Type;

/*@}*/ /* end of group Timer_A */


/******************************************************************************
* TLV Registers
******************************************************************************/
/** @addtogroup TLV MSP432P401R (TLV)
  @{
*/
typedef struct {
  _I  uint32_t TLV_CHECKSUM;                                                    /**< TLV Checksum */
  _I  uint32_t DEVICE_INFO_TAG;                                                 /**< Device Info Tag */
  _I  uint32_t DEVICE_INFO_LEN;                                                 /**< Device Info Length */
  _I  uint32_t DEVICE_ID;                                                       /**< Device ID */
  _I  uint32_t HWREV;                                                           /**< HW Revision */
  _I  uint32_t BCREV;                                                           /**< Boot Code Revision */
  _I  uint32_t ROM_DRVLIB_REV;                                                  /**< ROM Driver Library Revision */
  _I  uint32_t DIE_REC_TAG;                                                     /**< Die Record Tag */
  _I  uint32_t DIE_REC_LEN;                                                     /**< Die Record Length */
  _I  uint32_t DIE_XPOS;                                                        /**< Die X-Position */
  _I  uint32_t DIE_YPOS;                                                        /**< Die Y-Position */
  _I  uint32_t WAFER_ID;                                                        /**< Wafer ID */
  _I  uint32_t LOT_ID;                                                          /**< Lot ID */
  _I  uint32_t RESERVED0;                                                       /**< Reserved */
  _I  uint32_t RESERVED1;                                                       /**< Reserved */
  _I  uint32_t RESERVED2;                                                       /**< Reserved */
  _I  uint32_t TEST_RESULTS;                                                    /**< Test Results */
  _I  uint32_t CS_CAL_TAG;                                                      /**< Clock System Calibration Tag */
  _I  uint32_t CS_CAL_LEN;                                                      /**< Clock System Calibration Length */
  _I  uint32_t DCOIR_FCAL_RSEL04;                                               /**< DCO IR mode: Frequency calibration for DCORSEL 0 to 4 */
  _I  uint32_t DCOIR_FCAL_RSEL5;                                                /**< DCO IR mode: Frequency calibration for DCORSEL 5 */
  _I  uint32_t DCOIR_MAXPOSTUNE_RSEL04;                                         /**< DCO IR mode: Max Positive Tune for DCORSEL 0 to 4 */
  _I  uint32_t DCOIR_MAXNEGTUNE_RSEL04;                                         /**< DCO IR mode: Max Negative Tune for DCORSEL 0 to 4 */
  _I  uint32_t DCOIR_MAXPOSTUNE_RSEL5;                                          /**< DCO IR mode: Max Positive Tune for DCORSEL 5 */
  _I  uint32_t DCOIR_MAXNEGTUNE_RSEL5;                                          /**< DCO IR mode: Max Negative Tune for DCORSEL 5 */
  _I  uint32_t DCOIR_CONSTK_RSEL04;                                             /**< DCO IR mode: DCO Constant (K) for DCORSEL 0 to 4 */
  _I  uint32_t DCOIR_CONSTK_RSEL5;                                              /**< DCO IR mode: DCO Constant (K) for DCORSEL 5 */
  _I  uint32_t DCOER_FCAL_RSEL04;                                               /**< DCO ER mode: Frequency calibration for DCORSEL 0 to 4 */
  _I  uint32_t DCOER_FCAL_RSEL5;                                                /**< DCO ER mode: Frequency calibration for DCORSEL 5 */
  _I  uint32_t DCOER_MAXPOSTUNE_RSEL04;                                         /**< DCO ER mode: Max Positive Tune for DCORSEL 0 to 4 */
  _I  uint32_t DCOER_MAXNEGTUNE_RSEL04;                                         /**< DCO ER mode: Max Negative Tune for DCORSEL 0 to 4 */
  _I  uint32_t DCOER_MAXPOSTUNE_RSEL5;                                          /**< DCO ER mode: Max Positive Tune for DCORSEL 5 */
  _I  uint32_t DCOER_MAXNEGTUNE_RSEL5;                                          /**< DCO ER mode: Max Negative Tune for DCORSEL 5 */
  _I  uint32_t DCOER_CONSTK_RSEL04;                                             /**< DCO ER mode: DCO Constant (K) for DCORSEL 0 to 4 */
  _I  uint32_t DCOER_CONSTK_RSEL5;                                              /**< DCO ER mode: DCO Constant (K) for DCORSEL 5 */
  _I  uint32_t ADC14_CAL_TAG;                                                   /**< ADC14 Calibration Tag */
  _I  uint32_t ADC14_CAL_LEN;                                                   /**< ADC14 Calibration Length */
  _I  uint32_t ADC14_GF_EXTREF30C;                                              /**< ADC14 Gain Factor for External Reference 30�C */
  _I  uint32_t ADC14_GF_EXTREF85C;                                              /**< ADC14 Gain Factor for External Reference 85�C */
  _I  uint32_t ADC14_GF_BUF_EXTREF30C;                                          /**< ADC14 Gain Factor for Buffered External Reference 30�C */
  _I  uint32_t ADC14_GF_BUF_EXTREF85C;                                          /**< ADC14 Gain Factor for Buffered External Reference 85�C */
  _I  uint32_t ADC14_GF_BUF1P2V_INTREF30C_REFOUT0;                              /**< ADC14 Gain Factor for Buffered 1.2V Internal Reference 30�C (REFOUT = 0) */
  _I  uint32_t ADC14_GF_BUF1P2V_INTREF85C_REFOUT0;                              /**< ADC14 Gain Factor for Buffered 1.2V Internal Reference 85�C (REFOUT = 0) */
  _I  uint32_t ADC14_GF_BUF1P2V_INTREF30C_REFOUT1;                              /**< ADC14 Gain Factor for Buffered 1.2V Internal Reference 30�C (REFOUT = 1) */
  _I  uint32_t ADC14_GF_BUF1P2V_INTREF85C_REFOUT1;                              /**< ADC14 Gain Factor for Buffered 1.2V Internal Reference 85�C (REFOUT = 1) */
  _I  uint32_t ADC14_GF_BUF1P45V_INTREF30C_REFOUT0;                             /**< ADC14 Gain Factor for Buffered 1.45V Internal Reference 30�C (REFOUT = 0) */
  _I  uint32_t ADC14_GF_BUF1P45V_INTREF85C_REFOUT0;                             /**< ADC14 Gain Factor for Buffered 1.45V Internal Reference 85�C (REFOUT = 0) */
  _I  uint32_t ADC14_GF_BUF1P45V_INTREF30C_REFOUT1;                             /**< ADC14 Gain Factor for Buffered 1.45V Internal Reference 30�C (REFOUT = 1) */
  _I  uint32_t ADC14_GF_BUF1P45V_INTREF85C_REFOUT1;                             /**< ADC14 Gain Factor for Buffered 1.45V Internal Reference 85�C (REFOUT = 1) */
  _I  uint32_t ADC14_GF_BUF2P5V_INTREF30C_REFOUT0;                              /**< ADC14 Gain Factor for Buffered 2.5V Internal Reference 30�C (REFOUT = 0) */
  _I  uint32_t ADC14_GF_BUF2P5V_INTREF85C_REFOUT0;                              /**< ADC14 Gain Factor for Buffered 2.5V Internal Reference 85�C (REFOUT = 0) */
  _I  uint32_t ADC14_GF_BUF2P5V_INTREF30C_REFOUT1;                              /**< ADC14 Gain Factor for Buffered 2.5V Internal Reference 30�C (REFOUT = 1) */
  _I  uint32_t ADC14_GF_BUF2P5V_INTREF85C_REFOUT1;                              /**< ADC14 Gain Factor for Buffered 2.5V Internal Reference 85�C (REFOUT = 1) */
  _I  uint32_t ADC14_OFFSET_VRSEL_1;                                            /**< ADC14 Offset (ADC14VRSEL = 1h) */
  _I  uint32_t ADC14_OFFSET_VRSEL_E;                                            /**< ADC14 Offset (ADC14VRSEL = Eh) */
  _I  uint32_t ADC14_REF1P2V_TS30C;                                             /**< ADC14 1.2V Reference Temp. Sensor 30�C */
  _I  uint32_t ADC14_REF1P2V_TS85C;                                             /**< ADC14 1.2V Reference Temp. Sensor 85�C */
  _I  uint32_t ADC14_REF1P45V_TS30C;                                            /**< ADC14 1.45V Reference Temp. Sensor 30�C */
  _I  uint32_t ADC14_REF1P45V_TS85C;                                            /**< ADC14 1.45V Reference Temp. Sensor 85�C */
  _I  uint32_t ADC14_REF2P5V_TS30C;                                             /**< ADC14 2.5V Reference Temp. Sensor 30�C */
  _I  uint32_t ADC14_REF2P5V_TS85C;                                             /**< ADC14 2.5V Reference Temp. Sensor 85�C */
  _I  uint32_t REF_CAL_TAG;                                                     /**< REF Calibration Tag */
  _I  uint32_t REF_CAL_LEN;                                                     /**< REF Calibration Length */
  _I  uint32_t REF_1P2V;                                                        /**< REF 1.2V Reference */
  _I  uint32_t REF_1P45V;                                                       /**< REF 1.45V Reference */
  _I  uint32_t REF_2P5V;                                                        /**< REF 2.5V Reference */
  _I  uint32_t FLASH_INFO_TAG;                                                  /**< Flash Info Tag */
  _I  uint32_t FLASH_INFO_LEN;                                                  /**< Flash Info Length */
  _I  uint32_t FLASH_MAX_PROG_PULSES;                                           /**< Flash Maximum Programming Pulses */
  _I  uint32_t FLASH_MAX_ERASE_PULSES;                                          /**< Flash Maximum Erase Pulses */
  _I  uint32_t RANDOM_NUM_TAG;                                                  /**< 128-bit Random Number Tag */
  _I  uint32_t RANDOM_NUM_LEN;                                                  /**< 128-bit Random Number Length */
  _I  uint32_t RANDOM_NUM_1;                                                    /**< 32-bit Random Number 1 */
  _I  uint32_t RANDOM_NUM_2;                                                    /**< 32-bit Random Number 2 */
  _I  uint32_t RANDOM_NUM_3;                                                    /**< 32-bit Random Number 3 */
  _I  uint32_t RANDOM_NUM_4;                                                    /**< 32-bit Random Number 4 */
  _I  uint32_t BSL_CFG_TAG;                                                     /**< BSL Configuration Tag */
  _I  uint32_t BSL_CFG_LEN;                                                     /**< BSL Configuration Length */
  _I  uint32_t BSL_PERIPHIF_SEL;                                                /**< BSL Peripheral Interface Selection */
  _I  uint32_t BSL_PORTIF_CFG_UART;                                             /**< BSL Port Interface Configuration for UART */
  _I  uint32_t BSL_PORTIF_CFG_SPI;                                              /**< BSL Port Interface Configuration for SPI */
  _I  uint32_t BSL_PORTIF_CFG_I2C;                                              /**< BSL Port Interface Configuration for I2C */
  _I  uint32_t TLV_END;                                                         /**< TLV End Word */
} TLV_Type;
/*@}*/ /* end of group TLV */

/******************************************************************************
* WDT_A Registers
******************************************************************************/
/** @addtogroup WDT_A MSP432P401R (WDT_A)
  @{
*/
typedef struct {
       uint16_t RESERVED0[6];
  _IO uint16_t CTL;                                                             /**< Watchdog Timer Control Register */
} WDT_A_Type;
/*@}*/ /* end of group WDT_A */

#if defined ( __CC_ARM )
#pragma no_anon_unions
#endif

/*@}*/ /* end of group MSP432P401R_Peripherals */

/******************************************************************************
* Peripheral declaration                                                      *
******************************************************************************/
/** @addtogroup MSP432P401R_PeripheralDecl MSP432P401R Peripheral Declaration
  @{
*/
inline auto * const ADC14        = reinterpret_cast<ADC14_Type *>(ADC14_BASE);
inline auto * const AES256       = reinterpret_cast<AES256_Type *>(AES256_BASE);
inline auto * const CAPTIO0      = reinterpret_cast<CAPTIO_Type *>(CAPTIO0_BASE);
inline auto * const CAPTIO1      = reinterpret_cast<CAPTIO_Type *>(CAPTIO1_BASE);
inline auto * const COMP_E0      = reinterpret_cast<COMP_E_Type *>(COMP_E0_BASE);
inline auto * const COMP_E1      = reinterpret_cast<COMP_E_Type *>(COMP_E1_BASE);
inline auto * const CRC32        = reinterpret_cast<CRC32_Type *>(CRC32_BASE);
inline auto * const CS           = reinterpret_cast<CS_Type *>(CS_BASE);
inline auto * const PA           = reinterpret_cast<DIO_PORT_Interruptable_Type*>(DIO_BASE + 0x0000);
inline auto * const PB           = reinterpret_cast<DIO_PORT_Interruptable_Type*>(DIO_BASE + 0x0020);
inline auto * const PC           = reinterpret_cast<DIO_PORT_Interruptable_Type*>(DIO_BASE + 0x0040);
inline auto * const PD           = reinterpret_cast<DIO_PORT_Interruptable_Type*>(DIO_BASE + 0x0060);
inline auto * const PE           = reinterpret_cast<DIO_PORT_Interruptable_Type*>(DIO_BASE + 0x0080);
inline auto * const PJ           = reinterpret_cast<DIO_PORT_Not_Interruptable_Type*>(DIO_BASE + 0x0120);
inline auto * const P1           = reinterpret_cast<DIO_PORT_Odd_Interruptable_Type*>(DIO_BASE + 0x0000);
inline auto * const P2           = reinterpret_cast<DIO_PORT_Even_Interruptable_Type*>(DIO_BASE + 0x0000);
inline auto * const P3           = reinterpret_cast<DIO_PORT_Odd_Interruptable_Type*>(DIO_BASE + 0x0020);
inline auto * const P4           = reinterpret_cast<DIO_PORT_Even_Interruptable_Type*>(DIO_BASE + 0x0020);
inline auto * const P5           = reinterpret_cast<DIO_PORT_Odd_Interruptable_Type*>(DIO_BASE + 0x0040);
inline auto * const P6           = reinterpret_cast<DIO_PORT_Even_Interruptable_Type*>(DIO_BASE + 0x0040);
inline auto * const P7           = reinterpret_cast<DIO_PORT_Odd_Interruptable_Type*>(DIO_BASE + 0x0060);
inline auto * const P8           = reinterpret_cast<DIO_PORT_Even_Interruptable_Type*>(DIO_BASE + 0x0060);
inline auto * const P9           = reinterpret_cast<DIO_PORT_Odd_Interruptable_Type*>(DIO_BASE + 0x0080);
inline auto * const P10          = reinterpret_cast<DIO_PORT_Even_Interruptable_Type*>(DIO_BASE + 0x0080);
inline auto * const DMA_Channel  = reinterpret_cast<DMA_Channel_Type *>(DMA_BASE);
inline auto * const DMA_Control  = reinterpret_cast<DMA_Control_Type *>(DMA_BASE + 0x1000);
inline auto * const EUSCI_A0     = reinterpret_cast<EUSCI_A_Type *>(EUSCI_A0_BASE);
inline auto * const EUSCI_A0_SPI = reinterpret_cast<EUSCI_A_SPI_Type *>(EUSCI_A0_SPI_BASE);
inline auto * const EUSCI_A1     = reinterpret_cast<EUSCI_A_Type *>(EUSCI_A1_BASE);
inline auto * const EUSCI_A1_SPI = reinterpret_cast<EUSCI_A_SPI_Type *>(EUSCI_A1_SPI_BASE);
inline auto * const EUSCI_A2     = reinterpret_cast<EUSCI_A_Type *>(EUSCI_A2_BASE);
inline auto * const EUSCI_A2_SPI = reinterpret_cast<EUSCI_A_SPI_Type *>(EUSCI_A2_SPI_BASE);
inline auto * const EUSCI_A3     = reinterpret_cast<EUSCI_A_Type *>(EUSCI_A3_BASE);
inline auto * const EUSCI_A3_SPI = reinterpret_cast<EUSCI_A_SPI_Type *>(EUSCI_A3_SPI_BASE);
inline auto * const EUSCI_B0     = reinterpret_cast<EUSCI_B_Type *>(EUSCI_B0_BASE);
inline auto * const EUSCI_B0_SPI = reinterpret_cast<EUSCI_B_SPI_Type *>(EUSCI_B0_SPI_BASE);
inline auto * const EUSCI_B1     = reinterpret_cast<EUSCI_B_Type *>(EUSCI_B1_BASE);
inline auto * const EUSCI_B1_SPI = reinterpret_cast<EUSCI_B_SPI_Type *>(EUSCI_B1_SPI_BASE);
inline auto * const EUSCI_B2     = reinterpret_cast<EUSCI_B_Type *>(EUSCI_B2_BASE);
inline auto * const EUSCI_B2_SPI = reinterpret_cast<EUSCI_B_SPI_Type *>(EUSCI_B2_SPI_BASE);
inline auto * const EUSCI_B3     = reinterpret_cast<EUSCI_B_Type *>(EUSCI_B3_BASE);
inline auto * const EUSCI_B3_SPI = reinterpret_cast<EUSCI_B_SPI_Type *>(EUSCI_B3_SPI_BASE);
inline auto * const FLCTL        = reinterpret_cast<FLCTL_Type *>(FLCTL_BASE);
inline auto * const PCM          = reinterpret_cast<PCM_Type *>(PCM_BASE);
inline auto * const PMAP         = reinterpret_cast<PMAP_COMMON_Type*>(PMAP_BASE);
inline auto * const P1MAP        = reinterpret_cast<PMAP_REGISTER_Type *>(PMAP_BASE + 0x0008);
inline auto * const P2MAP        = reinterpret_cast<PMAP_REGISTER_Type *>(PMAP_BASE + 0x0010);
inline auto * const P3MAP        = reinterpret_cast<PMAP_REGISTER_Type *>(PMAP_BASE + 0x0018);
inline auto * const P4MAP        = reinterpret_cast<PMAP_REGISTER_Type *>(PMAP_BASE + 0x0020);
inline auto * const P5MAP        = reinterpret_cast<PMAP_REGISTER_Type *>(PMAP_BASE + 0x0028);
inline auto * const P6MAP        = reinterpret_cast<PMAP_REGISTER_Type *>(PMAP_BASE + 0x0030);
inline auto * const P7MAP        = reinterpret_cast<PMAP_REGISTER_Type *>(PMAP_BASE + 0x0038);
inline auto * const PSS          = reinterpret_cast<PSS_Type *>(PSS_BASE);
inline auto * const REF_A        = reinterpret_cast<REF_A_Type *>(REF_A_BASE);
inline auto * const RSTCTL       = reinterpret_cast<RSTCTL_Type *>(RSTCTL_BASE);
inline auto * const RTC_C        = reinterpret_cast<RTC_C_Type *>(RTC_C_BASE);
inline auto * const RTC_C_BCD    = reinterpret_cast<RTC_C_BCD_Type *>(RTC_C_BCD_BASE);
inline auto * const SYSCTL       = reinterpret_cast<SYSCTL_Type *>(SYSCTL_BASE);
inline auto * const SYSCTL_Boot  = reinterpret_cast<SYSCTL_Boot_Type *>(SYSCTL_BASE + 0x1000);
inline auto * const TIMER32_1    = reinterpret_cast<Timer32_Type *>(TIMER32_BASE);
inline auto * const TIMER32_2    = reinterpret_cast<Timer32_Type *>(TIMER32_BASE + 0x00020);
inline auto * const TIMER_A0     = reinterpret_cast<Timer_A_Type *>(TIMER_A0_BASE);
inline auto * const TIMER_A1     = reinterpret_cast<Timer_A_Type *>(TIMER_A1_BASE);
inline auto * const TIMER_A2     = reinterpret_cast<Timer_A_Type *>(TIMER_A2_BASE);
inline auto * const TIMER_A3     = reinterpret_cast<Timer_A_Type *>(TIMER_A3_BASE);
inline auto * const TLV          = reinterpret_cast<TLV_Type *>(TLV_BASE);
inline auto * const WDT_A        = reinterpret_cast<WDT_A_Type *>(WDT_A_BASE);
/*@}*/ /* end of group MSP432P401R_PeripheralDecl */

/*@}*/ /* end of group MSP432P401R_Definitions */

#endif /* __CMSIS_CONFIG__ */

#if defined (__cplusplus)
} // extern "C"
} // namespace sjsu::msp432p01r
#endif

#endif /* __MSP432P401R_H__ */
