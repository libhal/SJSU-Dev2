/**************************************************************************//**
 * @file      LPC17xx.h
 * @brief     CMSIS Cortex-M3 Core Peripheral Access Layer Header File for
 *            NXP LPC17xx Device Series
 * @version:  V1.09
 * @date:     17. March 2010
 * @modified: August 9, 2019
 *
 * @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#ifndef __LPC17xx_H__
#define __LPC17xx_H__
#pragma GCC system_header

/**
 * IO definitions
 *
 * define access restrictions to peripheral registers
 */

#ifdef __cplusplus
  #define     _I     volatile                /*!< defines 'read only' permissions      */
#else
  #define     _I     volatile const          /*!< defines 'read only' permissions      */
#endif
#define     _O     volatile                  /*!< defines 'write only' permissions     */
#define     _IO    volatile                  /*!< defines 'read / write' permissions   */
#include <stdint.h>

/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M3 Processor and Core Peripherals */
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          5         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    1         /*!< Set to 1 if different SysTick Config is used     */


/* Cortex-M3 processor and core peripherals           */
#include "L0_Platform/arm_cortex/m4/core_cm4.h"
// #include "system_LPC17xx.h"                 /* System Header */


#if defined (__cplusplus)
// SJSU-Dev2: Putting contents of this include in sjsu::lpc17xx
namespace sjsu::lpc17xx
{
extern "C" {
#endif
/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  Reset_IRQn                    = -15,      /*!< 1  Reset Vector, invoked on Power up and warm reset */
  NonMaskableInt_IRQn           = -14,      /*!< 2 Non Maskable Interrupt                         */
  MemoryManagement_IRQn         = -12,      /*!< 4 Cortex-M3 Memory Management Interrupt          */
  BusFault_IRQn                 = -11,      /*!< 5 Cortex-M3 Bus Fault Interrupt                  */
  UsageFault_IRQn               = -10,      /*!< 6 Cortex-M3 Usage Fault Interrupt                */
  SVCall_IRQn                   = -5,       /*!< 11 Cortex-M3 SV Call Interrupt                   */
  DebugMonitor_IRQn             = -4,       /*!< 12 Cortex-M3 Debug Monitor Interrupt             */
  PendSV_IRQn                   = -2,       /*!< 14 Cortex-M3 Pend SV Interrupt                   */
  SysTick_IRQn                  = -1,       /*!< 15 Cortex-M3 System Tick Interrupt               */

/******  LPC17xx Specific Interrupt Numbers *******************************************************/
  WDT_IRQn                      = 0,        /*!< Watchdog Timer Interrupt                         */
  TIMER0_IRQn                   = 1,        /*!< Timer0 Interrupt                                 */
  TIMER1_IRQn                   = 2,        /*!< Timer1 Interrupt                                 */
  TIMER2_IRQn                   = 3,        /*!< Timer2 Interrupt                                 */
  TIMER3_IRQn                   = 4,        /*!< Timer3 Interrupt                                 */
  UART0_IRQn                    = 5,        /*!< UART0 Interrupt                                  */
  UART1_IRQn                    = 6,        /*!< UART1 Interrupt                                  */
  UART2_IRQn                    = 7,        /*!< UART2 Interrupt                                  */
  UART3_IRQn                    = 8,        /*!< UART3 Interrupt                                  */
  PWM1_IRQn                     = 9,        /*!< PWM1 Interrupt                                   */
  I2C0_IRQn                     = 10,       /*!< I2C0 Interrupt                                   */
  I2C1_IRQn                     = 11,       /*!< I2C1 Interrupt                                   */
  I2C2_IRQn                     = 12,       /*!< I2C2 Interrupt                                   */
  SPI_IRQn                      = 13,       /*!< SPI Interrupt                                    */
  SSP0_IRQn                     = 14,       /*!< SSP0 Interrupt                                   */
  SSP1_IRQn                     = 15,       /*!< SSP1 Interrupt                                   */
  PLL0_IRQn                     = 16,       /*!< PLL0 Lock (Main PLL) Interrupt                   */
  RTC_IRQn                      = 17,       /*!< Real Time Clock Interrupt                        */
  EINT0_IRQn                    = 18,       /*!< External Interrupt 0 Interrupt                   */
  EINT1_IRQn                    = 19,       /*!< External Interrupt 1 Interrupt                   */
  EINT2_IRQn                    = 20,       /*!< External Interrupt 2 Interrupt                   */
  EINT3_IRQn                    = 21,       /*!< External Interrupt 3 Interrupt                   */
  ADC_IRQn                      = 22,       /*!< A/D Converter Interrupt                          */
  BOD_IRQn                      = 23,       /*!< Brown-Out Detect Interrupt                       */
  USB_IRQn                      = 24,       /*!< USB Interrupt                                    */
  CAN_IRQn                      = 25,       /*!< CAN Interrupt                                    */
  DMA_IRQn                      = 26,       /*!< General Purpose DMA Interrupt                    */
  I2S_IRQn                      = 27,       /*!< I2S Interrupt                                    */
  ENET_IRQn                     = 28,       /*!< Ethernet Interrupt                               */
  RIT_IRQn                      = 29,       /*!< Repetitive Interrupt Timer Interrupt             */
  MCPWM_IRQn                    = 30,       /*!< Motor Control PWM Interrupt                      */
  QEI_IRQn                      = 31,       /*!< Quadrature Encoder Interface Interrupt           */
  PLL1_IRQn                     = 32,       /*!< PLL1 Lock (USB PLL) Interrupt                    */
  USBActivity_IRQn              = 33,       /* USB Activity interrupt                             */
  CANActivity_IRQn              = 34,       /* CAN Activity interrupt                             */
  kNumberOfIrqs
};

/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*------------- System Control (SC) ------------------------------------------*/
typedef struct
{
  _IO uint32_t FLASHCFG;               /* Flash Accelerator Module           */
       uint32_t RESERVED0[31];
  _IO uint32_t PLL0CON;                /* Clocking and Power Control         */
  _IO uint32_t PLL0CFG;
  _I  uint32_t PLL0STAT;
  _O  uint32_t PLL0FEED;
       uint32_t RESERVED1[4];
  _IO uint32_t PLL1CON;
  _IO uint32_t PLL1CFG;
  _I  uint32_t PLL1STAT;
  _O  uint32_t PLL1FEED;
       uint32_t RESERVED2[4];
  _IO uint32_t PCON;
  _IO uint32_t PCONP;
       uint32_t RESERVED3[15];
  _IO uint32_t CCLKCFG;
  _IO uint32_t USBCLKCFG;
  _IO uint32_t CLKSRCSEL;
  _IO uint32_t	CANSLEEPCLR;
  _IO uint32_t	CANWAKEFLAGS;
       uint32_t RESERVED4[10];
  _IO uint32_t EXTINT;                 /* External Interrupts                */
       uint32_t RESERVED5;
  _IO uint32_t EXTMODE;
  _IO uint32_t EXTPOLAR;
       uint32_t RESERVED6[12];
  _IO uint32_t RSID;                   /* Reset                              */
       uint32_t RESERVED7[7];
  _IO uint32_t SCS;                    /* Syscon Miscellaneous Registers     */
  _IO uint32_t IRCTRIM;                /* Clock Dividers                     */
  _IO uint32_t PCLKSEL0;
  _IO uint32_t PCLKSEL1;
       uint32_t RESERVED8[4];
  _IO uint32_t USBIntSt;               /* USB Device/OTG Interrupt Register  */
  _IO uint32_t DMAREQSEL;
  _IO uint32_t CLKOUTCFG;              /* Clock Output Configuration         */
 } LPC_SC_TypeDef;

/*------------- Pin Connect Block (PINCON) -----------------------------------*/
typedef struct
{
  _IO uint32_t PINSEL0;
  _IO uint32_t PINSEL1;
  _IO uint32_t PINSEL2;
  _IO uint32_t PINSEL3;
  _IO uint32_t PINSEL4;
  _IO uint32_t PINSEL5;
  _IO uint32_t PINSEL6;
  _IO uint32_t PINSEL7;
  _IO uint32_t PINSEL8;
  _IO uint32_t PINSEL9;
  _IO uint32_t PINSEL10;
       uint32_t RESERVED0[5];
  _IO uint32_t PINMODE0;
  _IO uint32_t PINMODE1;
  _IO uint32_t PINMODE2;
  _IO uint32_t PINMODE3;
  _IO uint32_t PINMODE4;
  _IO uint32_t PINMODE5;
  _IO uint32_t PINMODE6;
  _IO uint32_t PINMODE7;
  _IO uint32_t PINMODE8;
  _IO uint32_t PINMODE9;
  _IO uint32_t PINMODE_OD0;
  _IO uint32_t PINMODE_OD1;
  _IO uint32_t PINMODE_OD2;
  _IO uint32_t PINMODE_OD3;
  _IO uint32_t PINMODE_OD4;
  _IO uint32_t I2CPADCFG;
} LPC_PINCON_TypeDef;

/*------------- General Purpose Input/Output (GPIO) --------------------------*/
typedef struct
{
  union {
    _IO uint32_t FIODIR;
    struct {
      _IO uint16_t FIODIRL;
      _IO uint16_t FIODIRH;
    };
    struct {
      _IO uint8_t  FIODIR0;
      _IO uint8_t  FIODIR1;
      _IO uint8_t  FIODIR2;
      _IO uint8_t  FIODIR3;
    };
  };
  uint32_t RESERVED0[3];
  union {
    _IO uint32_t FIOMASK;
    struct {
      _IO uint16_t FIOMASKL;
      _IO uint16_t FIOMASKH;
    };
    struct {
      _IO uint8_t  FIOMASK0;
      _IO uint8_t  FIOMASK1;
      _IO uint8_t  FIOMASK2;
      _IO uint8_t  FIOMASK3;
    };
  };
  union {
    _IO uint32_t FIOPIN;
    struct {
      _IO uint16_t FIOPINL;
      _IO uint16_t FIOPINH;
    };
    struct {
      _IO uint8_t  FIOPIN0;
      _IO uint8_t  FIOPIN1;
      _IO uint8_t  FIOPIN2;
      _IO uint8_t  FIOPIN3;
    };
  };
  union {
    _IO uint32_t FIOSET;
    struct {
      _IO uint16_t FIOSETL;
      _IO uint16_t FIOSETH;
    };
    struct {
      _IO uint8_t  FIOSET0;
      _IO uint8_t  FIOSET1;
      _IO uint8_t  FIOSET2;
      _IO uint8_t  FIOSET3;
    };
  };
  union {
    _O  uint32_t FIOCLR;
    struct {
      _O  uint16_t FIOCLRL;
      _O  uint16_t FIOCLRH;
    };
    struct {
      _O  uint8_t  FIOCLR0;
      _O  uint8_t  FIOCLR1;
      _O  uint8_t  FIOCLR2;
      _O  uint8_t  FIOCLR3;
    };
  };
} LPC_GPIO_TypeDef;

typedef struct
{
  _I  uint32_t IntStatus;
  _I  uint32_t IO0IntStatR;
  _I  uint32_t IO0IntStatF;
  _O  uint32_t IO0IntClr;
  _IO uint32_t IO0IntEnR;
  _IO uint32_t IO0IntEnF;
       uint32_t RESERVED0[3];
  _I  uint32_t IO2IntStatR;
  _I  uint32_t IO2IntStatF;
  _O  uint32_t IO2IntClr;
  _IO uint32_t IO2IntEnR;
  _IO uint32_t IO2IntEnF;
} LPC_GPIOINT_TypeDef;

/*------------- Timer (TIM) --------------------------------------------------*/
typedef struct
{
  _IO uint32_t IR;
  _IO uint32_t TCR;
  _IO uint32_t TC;
  _IO uint32_t PR;
  _IO uint32_t PC;
  _IO uint32_t MCR;
  _IO uint32_t MR0;
  _IO uint32_t MR1;
  _IO uint32_t MR2;
  _IO uint32_t MR3;
  _IO uint32_t CCR;
  _I  uint32_t CR0;
  _I  uint32_t CR1;
       uint32_t RESERVED0[2];
  _IO uint32_t EMR;
       uint32_t RESERVED1[12];
  _IO uint32_t CTCR;
} LPC_TIM_TypeDef;

/*------------- Pulse-Width Modulation (PWM) ---------------------------------*/
typedef struct
{
  _IO uint32_t IR;
  _IO uint32_t TCR;
  _IO uint32_t TC;
  _IO uint32_t PR;
  _IO uint32_t PC;
  _IO uint32_t MCR;
  _IO uint32_t MR0;
  _IO uint32_t MR1;
  _IO uint32_t MR2;
  _IO uint32_t MR3;
  _IO uint32_t CCR;
  _I  uint32_t CR0;
  _I  uint32_t CR1;
  _I  uint32_t CR2;
  _I  uint32_t CR3;
       uint32_t RESERVED0;
  _IO uint32_t MR4;
  _IO uint32_t MR5;
  _IO uint32_t MR6;
  _IO uint32_t PCR;
  _IO uint32_t LER;
       uint32_t RESERVED1[7];
  _IO uint32_t CTCR;
} LPC_PWM_TypeDef;

/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
typedef struct
{
  union {
  _I  uint8_t  RBR;
  _O  uint8_t  THR;
  _IO uint8_t  DLL;
       uint32_t RESERVED0;
  };
  union {
  _IO uint8_t  DLM;
  _IO uint32_t IER;
  };
  union {
  _I  uint32_t IIR;
  _O  uint8_t  FCR;
  };
  _IO uint8_t  LCR;
       uint8_t  RESERVED1[7];
  _I  uint8_t  LSR;
       uint8_t  RESERVED2[7];
  _IO uint8_t  SCR;
       uint8_t  RESERVED3[3];
  _IO uint32_t ACR;
  _IO uint8_t  ICR;
       uint8_t  RESERVED4[3];
  _IO uint8_t  FDR;
       uint8_t  RESERVED5[7];
  _IO uint8_t  TER;
       uint8_t  RESERVED6[39];
  _IO uint32_t FIFOLVL;
} LPC_UART_TypeDef;

typedef struct
{
  union {
  _I  uint8_t  RBR;
  _O  uint8_t  THR;
  _IO uint8_t  DLL;
       uint32_t RESERVED0;
  };
  union {
  _IO uint8_t  DLM;
  _IO uint32_t IER;
  };
  union {
  _I  uint32_t IIR;
  _O  uint8_t  FCR;
  };
  _IO uint8_t  LCR;
       uint8_t  RESERVED1[7];
  _I  uint8_t  LSR;
       uint8_t  RESERVED2[7];
  _IO uint8_t  SCR;
       uint8_t  RESERVED3[3];
  _IO uint32_t ACR;
  _IO uint8_t  ICR;
       uint8_t  RESERVED4[3];
  _IO uint8_t  FDR;
       uint8_t  RESERVED5[7];
  _IO uint8_t  TER;
       uint8_t  RESERVED6[39];
  _IO uint32_t FIFOLVL;
} LPC_UART0_TypeDef;

typedef struct
{
  union {
  _I  uint8_t  RBR;
  _O  uint8_t  THR;
  _IO uint8_t  DLL;
       uint32_t RESERVED0;
  };
  union {
  _IO uint8_t  DLM;
  _IO uint32_t IER;
  };
  union {
  _I  uint32_t IIR;
  _O  uint8_t  FCR;
  };
  _IO uint8_t  LCR;
       uint8_t  RESERVED1[3];
  _IO uint8_t  MCR;
       uint8_t  RESERVED2[3];
  _I  uint8_t  LSR;
       uint8_t  RESERVED3[3];
  _I  uint8_t  MSR;
       uint8_t  RESERVED4[3];
  _IO uint8_t  SCR;
       uint8_t  RESERVED5[3];
  _IO uint32_t ACR;
       uint32_t RESERVED6;
  _IO uint32_t FDR;
       uint32_t RESERVED7;
  _IO uint8_t  TER;
       uint8_t  RESERVED8[27];
  _IO uint8_t  RS485CTRL;
       uint8_t  RESERVED9[3];
  _IO uint8_t  ADRMATCH;
       uint8_t  RESERVED10[3];
  _IO uint8_t  RS485DLY;
       uint8_t  RESERVED11[3];
  _IO uint32_t FIFOLVL;
} LPC_UART1_TypeDef;

/*------------- Serial Peripheral Interface (SPI) ----------------------------*/
typedef struct
{
  _IO uint32_t SPCR;
  _I  uint32_t SPSR;
  _IO uint32_t SPDR;
  _IO uint32_t SPCCR;
       uint32_t RESERVED0[3];
  _IO uint32_t SPINT;
} LPC_SPI_TypeDef;

/*------------- Synchronous Serial Communication (SSP) -----------------------*/
typedef struct
{
  _IO uint32_t CR0;
  _IO uint32_t CR1;
  _IO uint32_t DR;
  _I  uint32_t SR;
  _IO uint32_t CPSR;
  _IO uint32_t IMSC;
  _IO uint32_t RIS;
  _IO uint32_t MIS;
  _IO uint32_t ICR;
  _IO uint32_t DMACR;
} LPC_SSP_TypeDef;

/*------------- Inter-Integrated Circuit (I2C) -------------------------------*/
typedef struct
{
  _IO uint32_t I2CONSET;
  _I  uint32_t I2STAT;
  _IO uint32_t I2DAT;
  _IO uint32_t I2ADR0;
  _IO uint32_t I2SCLH;
  _IO uint32_t I2SCLL;
  _O  uint32_t I2CONCLR;
  _IO uint32_t MMCTRL;
  _IO uint32_t I2ADR1;
  _IO uint32_t I2ADR2;
  _IO uint32_t I2ADR3;
  _I  uint32_t I2DATA_BUFFER;
  _IO uint32_t I2MASK0;
  _IO uint32_t I2MASK1;
  _IO uint32_t I2MASK2;
  _IO uint32_t I2MASK3;
} LPC_I2C_TypeDef;

/*------------- Inter IC Sound (I2S) -----------------------------------------*/
typedef struct
{
  _IO uint32_t I2SDAO;
  _IO uint32_t I2SDAI;
  _O  uint32_t I2STXFIFO;
  _I  uint32_t I2SRXFIFO;
  _I  uint32_t I2SSTATE;
  _IO uint32_t I2SDMA1;
  _IO uint32_t I2SDMA2;
  _IO uint32_t I2SIRQ;
  _IO uint32_t I2STXRATE;
  _IO uint32_t I2SRXRATE;
  _IO uint32_t I2STXBITRATE;
  _IO uint32_t I2SRXBITRATE;
  _IO uint32_t I2STXMODE;
  _IO uint32_t I2SRXMODE;
} LPC_I2S_TypeDef;

/*------------- Repetitive Interrupt Timer (RIT) -----------------------------*/
typedef struct
{
  _IO uint32_t RICOMPVAL;
  _IO uint32_t RIMASK;
  _IO uint8_t  RICTRL;
       uint8_t  RESERVED0[3];
  _IO uint32_t RICOUNTER;
} LPC_RIT_TypeDef;

/*------------- Real-Time Clock (RTC) ----------------------------------------*/
typedef struct
{
  _IO uint8_t  ILR;
       uint8_t  RESERVED0[7];
  _IO uint8_t  CCR;
       uint8_t  RESERVED1[3];
  _IO uint8_t  CIIR;
       uint8_t  RESERVED2[3];
  _IO uint8_t  AMR;
       uint8_t  RESERVED3[3];
  _I  uint32_t CTIME0;
  _I  uint32_t CTIME1;
  _I  uint32_t CTIME2;
  _IO uint8_t  SEC;
       uint8_t  RESERVED4[3];
  _IO uint8_t  MIN;
       uint8_t  RESERVED5[3];
  _IO uint8_t  HOUR;
       uint8_t  RESERVED6[3];
  _IO uint8_t  DOM;
       uint8_t  RESERVED7[3];
  _IO uint8_t  DOW;
       uint8_t  RESERVED8[3];
  _IO uint16_t DOY;
       uint16_t RESERVED9;
  _IO uint8_t  MONTH;
       uint8_t  RESERVED10[3];
  _IO uint16_t YEAR;
       uint16_t RESERVED11;
  _IO uint32_t CALIBRATION;
  _IO uint32_t GPREG0;
  _IO uint32_t GPREG1;
  _IO uint32_t GPREG2;
  _IO uint32_t GPREG3;
  _IO uint32_t GPREG4;
  _IO uint8_t  RTC_AUXEN;
       uint8_t  RESERVED12[3];
  _IO uint8_t  RTC_AUX;
       uint8_t  RESERVED13[3];
  _IO uint8_t  ALSEC;
       uint8_t  RESERVED14[3];
  _IO uint8_t  ALMIN;
       uint8_t  RESERVED15[3];
  _IO uint8_t  ALHOUR;
       uint8_t  RESERVED16[3];
  _IO uint8_t  ALDOM;
       uint8_t  RESERVED17[3];
  _IO uint8_t  ALDOW;
       uint8_t  RESERVED18[3];
  _IO uint16_t ALDOY;
       uint16_t RESERVED19;
  _IO uint8_t  ALMON;
       uint8_t  RESERVED20[3];
  _IO uint16_t ALYEAR;
       uint16_t RESERVED21;
} LPC_RTC_TypeDef;

/*------------- Watchdog Timer (WDT) -----------------------------------------*/
typedef struct
{
  _IO uint8_t  WDMOD;
       uint8_t  RESERVED0[3];
  _IO uint32_t WDTC;
  _O  uint8_t  WDFEED;
       uint8_t  RESERVED1[3];
  _I  uint32_t WDTV;
  _IO uint32_t WDCLKSEL;
} LPC_WDT_TypeDef;

/*------------- Analog-to-Digital Converter (ADC) ----------------------------*/
typedef struct
{
  _IO uint32_t ADCR;
  _IO uint32_t ADGDR;
       uint32_t RESERVED0;
  _IO uint32_t ADINTEN;
  _I  uint32_t ADDR0;
  _I  uint32_t ADDR1;
  _I  uint32_t ADDR2;
  _I  uint32_t ADDR3;
  _I  uint32_t ADDR4;
  _I  uint32_t ADDR5;
  _I  uint32_t ADDR6;
  _I  uint32_t ADDR7;
  _I  uint32_t ADSTAT;
  _IO uint32_t ADTRM;
} LPC_ADC_TypeDef;

/*------------- Digital-to-Analog Converter (DAC) ----------------------------*/
typedef struct
{
  _IO uint32_t DACR;
  _IO uint32_t DACCTRL;
  _IO uint16_t DACCNTVAL;
} LPC_DAC_TypeDef;

/*------------- Motor Control Pulse-Width Modulation (MCPWM) -----------------*/
typedef struct
{
  _I  uint32_t MCCON;
  _O  uint32_t MCCON_SET;
  _O  uint32_t MCCON_CLR;
  _I  uint32_t MCCAPCON;
  _O  uint32_t MCCAPCON_SET;
  _O  uint32_t MCCAPCON_CLR;
  _IO uint32_t MCTIM0;
  _IO uint32_t MCTIM1;
  _IO uint32_t MCTIM2;
  _IO uint32_t MCPER0;
  _IO uint32_t MCPER1;
  _IO uint32_t MCPER2;
  _IO uint32_t MCPW0;
  _IO uint32_t MCPW1;
  _IO uint32_t MCPW2;
  _IO uint32_t MCDEADTIME;
  _IO uint32_t MCCCP;
  _IO uint32_t MCCR0;
  _IO uint32_t MCCR1;
  _IO uint32_t MCCR2;
  _I  uint32_t MCINTEN;
  _O  uint32_t MCINTEN_SET;
  _O  uint32_t MCINTEN_CLR;
  _I  uint32_t MCCNTCON;
  _O  uint32_t MCCNTCON_SET;
  _O  uint32_t MCCNTCON_CLR;
  _I  uint32_t MCINTFLAG;
  _O  uint32_t MCINTFLAG_SET;
  _O  uint32_t MCINTFLAG_CLR;
  _O  uint32_t MCCAP_CLR;
} LPC_MCPWM_TypeDef;

/*------------- Quadrature Encoder Interface (QEI) ---------------------------*/
typedef struct
{
  _O  uint32_t QEICON;
  _I  uint32_t QEISTAT;
  _IO uint32_t QEICONF;
  _I  uint32_t QEIPOS;
  _IO uint32_t QEIMAXPOS;
  _IO uint32_t CMPOS0;
  _IO uint32_t CMPOS1;
  _IO uint32_t CMPOS2;
  _I  uint32_t INXCNT;
  _IO uint32_t INXCMP;
  _IO uint32_t QEILOAD;
  _I  uint32_t QEITIME;
  _I  uint32_t QEIVEL;
  _I  uint32_t QEICAP;
  _IO uint32_t VELCOMP;
  _IO uint32_t FILTER;
       uint32_t RESERVED0[998];
  _O  uint32_t QEIIEC;
  _O  uint32_t QEIIES;
  _I  uint32_t QEIINTSTAT;
  _I  uint32_t QEIIE;
  _O  uint32_t QEICLR;
  _O  uint32_t QEISET;
} LPC_QEI_TypeDef;

/*------------- Controller Area Network (CAN) --------------------------------*/
typedef struct
{
  _IO uint32_t mask[512];              /* ID Masks                           */
} LPC_CANAF_RAM_TypeDef;

typedef struct                          /* Acceptance Filter Registers        */
{
  _IO uint32_t AFMR;
  _IO uint32_t SFF_sa;
  _IO uint32_t SFF_GRP_sa;
  _IO uint32_t EFF_sa;
  _IO uint32_t EFF_GRP_sa;
  _IO uint32_t ENDofTable;
  _I  uint32_t LUTerrAd;
  _I  uint32_t LUTerr;
  _IO uint32_t FCANIE;
  _IO uint32_t FCANIC0;
  _IO uint32_t FCANIC1;
} LPC_CANAF_TypeDef;

typedef struct                          /* Central Registers                  */
{
  _I  uint32_t CANTxSR;
  _I  uint32_t CANRxSR;
  _I  uint32_t CANMSR;
} LPC_CANCR_TypeDef;

typedef struct                          /* Controller Registers               */
{
  _IO uint32_t MOD;
  _O  uint32_t CMR;
  _IO uint32_t GSR;
  _I  uint32_t ICR;
  _IO uint32_t IER;
  _IO uint32_t BTR;
  _IO uint32_t EWL;
  _I  uint32_t SR;
  _IO uint32_t RFS;
  _IO uint32_t RID;
  _IO uint32_t RDA;
  _IO uint32_t RDB;
  _IO uint32_t TFI1;
  _IO uint32_t TID1;
  _IO uint32_t TDA1;
  _IO uint32_t TDB1;
  _IO uint32_t TFI2;
  _IO uint32_t TID2;
  _IO uint32_t TDA2;
  _IO uint32_t TDB2;
  _IO uint32_t TFI3;
  _IO uint32_t TID3;
  _IO uint32_t TDA3;
  _IO uint32_t TDB3;
} LPC_CAN_TypeDef;

/*------------- General Purpose Direct Memory Access (GPDMA) -----------------*/
typedef struct                          /* Common Registers                   */
{
  _I  uint32_t DMACIntStat;
  _I  uint32_t DMACIntTCStat;
  _O  uint32_t DMACIntTCClear;
  _I  uint32_t DMACIntErrStat;
  _O  uint32_t DMACIntErrClr;
  _I  uint32_t DMACRawIntTCStat;
  _I  uint32_t DMACRawIntErrStat;
  _I  uint32_t DMACEnbldChns;
  _IO uint32_t DMACSoftBReq;
  _IO uint32_t DMACSoftSReq;
  _IO uint32_t DMACSoftLBReq;
  _IO uint32_t DMACSoftLSReq;
  _IO uint32_t DMACConfig;
  _IO uint32_t DMACSync;
} LPC_GPDMA_TypeDef;

typedef struct                          /* Channel Registers                  */
{
  _IO uint32_t DMACCSrcAddr;
  _IO uint32_t DMACCDestAddr;
  _IO uint32_t DMACCLLI;
  _IO uint32_t DMACCControl;
  _IO uint32_t DMACCConfig;
} LPC_GPDMACH_TypeDef;

/*------------- Universal Serial Bus (USB) -----------------------------------*/
typedef struct
{
  _I  uint32_t HcRevision;             /* USB Host Registers                 */
  _IO uint32_t HcControl;
  _IO uint32_t HcCommandStatus;
  _IO uint32_t HcInterruptStatus;
  _IO uint32_t HcInterruptEnable;
  _IO uint32_t HcInterruptDisable;
  _IO uint32_t HcHCCA;
  _I  uint32_t HcPeriodCurrentED;
  _IO uint32_t HcControlHeadED;
  _IO uint32_t HcControlCurrentED;
  _IO uint32_t HcBulkHeadED;
  _IO uint32_t HcBulkCurrentED;
  _I  uint32_t HcDoneHead;
  _IO uint32_t HcFmInterval;
  _I  uint32_t HcFmRemaining;
  _I  uint32_t HcFmNumber;
  _IO uint32_t HcPeriodicStart;
  _IO uint32_t HcLSTreshold;
  _IO uint32_t HcRhDescriptorA;
  _IO uint32_t HcRhDescriptorB;
  _IO uint32_t HcRhStatus;
  _IO uint32_t HcRhPortStatus1;
  _IO uint32_t HcRhPortStatus2;
       uint32_t RESERVED0[40];
  _I  uint32_t Module_ID;

  _I  uint32_t OTGIntSt;               /* USB On-The-Go Registers            */
  _IO uint32_t OTGIntEn;
  _O  uint32_t OTGIntSet;
  _O  uint32_t OTGIntClr;
  _IO uint32_t OTGStCtrl;
  _IO uint32_t OTGTmr;
       uint32_t RESERVED1[58];

  _I  uint32_t USBDevIntSt;            /* USB Device Interrupt Registers     */
  _IO uint32_t USBDevIntEn;
  _O  uint32_t USBDevIntClr;
  _O  uint32_t USBDevIntSet;

  _O  uint32_t USBCmdCode;             /* USB Device SIE Command Registers   */
  _I  uint32_t USBCmdData;

  _I  uint32_t USBRxData;              /* USB Device Transfer Registers      */
  _O  uint32_t USBTxData;
  _I  uint32_t USBRxPLen;
  _O  uint32_t USBTxPLen;
  _IO uint32_t USBCtrl;
  _O  uint32_t USBDevIntPri;

  _I  uint32_t USBEpIntSt;             /* USB Device Endpoint Interrupt Regs */
  _IO uint32_t USBEpIntEn;
  _O  uint32_t USBEpIntClr;
  _O  uint32_t USBEpIntSet;
  _O  uint32_t USBEpIntPri;

  _IO uint32_t USBReEp;                /* USB Device Endpoint Realization Reg*/
  _O  uint32_t USBEpInd;
  _IO uint32_t USBMaxPSize;

  _I  uint32_t USBDMARSt;              /* USB Device DMA Registers           */
  _O  uint32_t USBDMARClr;
  _O  uint32_t USBDMARSet;
       uint32_t RESERVED2[9];
  _IO uint32_t USBUDCAH;
  _I  uint32_t USBEpDMASt;
  _O  uint32_t USBEpDMAEn;
  _O  uint32_t USBEpDMADis;
  _I  uint32_t USBDMAIntSt;
  _IO uint32_t USBDMAIntEn;
       uint32_t RESERVED3[2];
  _I  uint32_t USBEoTIntSt;
  _O  uint32_t USBEoTIntClr;
  _O  uint32_t USBEoTIntSet;
  _I  uint32_t USBNDDRIntSt;
  _O  uint32_t USBNDDRIntClr;
  _O  uint32_t USBNDDRIntSet;
  _I  uint32_t USBSysErrIntSt;
  _O  uint32_t USBSysErrIntClr;
  _O  uint32_t USBSysErrIntSet;
       uint32_t RESERVED4[15];

  union {
  _I  uint32_t I2C_RX;                 /* USB OTG I2C Registers              */
  _O  uint32_t I2C_TX;
  };
  _I  uint32_t I2C_STS;
  _IO uint32_t I2C_CTL;
  _IO uint32_t I2C_CLKHI;
  _O  uint32_t I2C_CLKLO;
       uint32_t RESERVED5[824];

  union {
  _IO uint32_t USBClkCtrl;             /* USB Clock Control Registers        */
  _IO uint32_t OTGClkCtrl;
  };
  union {
  _I  uint32_t USBClkSt;
  _I  uint32_t OTGClkSt;
  };
} LPC_USB_TypeDef;

/*------------- Ethernet Media Access Controller (EMAC) ----------------------*/
typedef struct
{
  _IO uint32_t MAC1;                   /* MAC Registers                      */
  _IO uint32_t MAC2;
  _IO uint32_t IPGT;
  _IO uint32_t IPGR;
  _IO uint32_t CLRT;
  _IO uint32_t MAXF;
  _IO uint32_t SUPP;
  _IO uint32_t TEST;
  _IO uint32_t MCFG;
  _IO uint32_t MCMD;
  _IO uint32_t MADR;
  _O  uint32_t MWTD;
  _I  uint32_t MRDD;
  _I  uint32_t MIND;
       uint32_t RESERVED0[2];
  _IO uint32_t SA0;
  _IO uint32_t SA1;
  _IO uint32_t SA2;
       uint32_t RESERVED1[45];
  _IO uint32_t Command;                /* Control Registers                  */
  _I  uint32_t Status;
  _IO uint32_t RxDescriptor;
  _IO uint32_t RxStatus;
  _IO uint32_t RxDescriptorNumber;
  _I  uint32_t RxProduceIndex;
  _IO uint32_t RxConsumeIndex;
  _IO uint32_t TxDescriptor;
  _IO uint32_t TxStatus;
  _IO uint32_t TxDescriptorNumber;
  _IO uint32_t TxProduceIndex;
  _I  uint32_t TxConsumeIndex;
       uint32_t RESERVED2[10];
  _I  uint32_t TSV0;
  _I  uint32_t TSV1;
  _I  uint32_t RSV;
       uint32_t RESERVED3[3];
  _IO uint32_t FlowControlCounter;
  _I  uint32_t FlowControlStatus;
       uint32_t RESERVED4[34];
  _IO uint32_t RxFilterCtrl;           /* Rx Filter Registers                */
  _IO uint32_t RxFilterWoLStatus;
  _IO uint32_t RxFilterWoLClear;
       uint32_t RESERVED5;
  _IO uint32_t HashFilterL;
  _IO uint32_t HashFilterH;
       uint32_t RESERVED6[882];
  _I  uint32_t IntStatus;              /* Module Control Registers           */
  _IO uint32_t IntEnable;
  _O  uint32_t IntClear;
  _O  uint32_t IntSet;
       uint32_t RESERVED7;
  _IO uint32_t PowerDown;
       uint32_t RESERVED8;
  _IO uint32_t Module_ID;
} LPC_EMAC_TypeDef;

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* Base addresses                                                             */
constexpr intptr_t LPC_FLASH_BASE = 0x00000000UL;
constexpr intptr_t LPC_RAM_BASE   = 0x10000000UL;
constexpr intptr_t LPC_GPIO_BASE  = 0x2009C000UL;
constexpr intptr_t LPC_APB0_BASE  = 0x40000000UL;
constexpr intptr_t LPC_APB1_BASE  = 0x40080000UL;
constexpr intptr_t LPC_AHB_BASE   = 0x50000000UL;
constexpr intptr_t LPC_CM3_BASE   = 0xE0000000UL;

/* APB0 peripherals                                                           */
constexpr intptr_t LPC_WDT_BASE       = (LPC_APB0_BASE + 0x00000);
constexpr intptr_t LPC_TIM0_BASE      = (LPC_APB0_BASE + 0x04000);
constexpr intptr_t LPC_TIM1_BASE      = (LPC_APB0_BASE + 0x08000);
constexpr intptr_t LPC_UART0_BASE     = (LPC_APB0_BASE + 0x0C000);
constexpr intptr_t LPC_UART1_BASE     = (LPC_APB0_BASE + 0x10000);
constexpr intptr_t LPC_PWM1_BASE      = (LPC_APB0_BASE + 0x18000);
constexpr intptr_t LPC_I2C0_BASE      = (LPC_APB0_BASE + 0x1C000);
constexpr intptr_t LPC_SPI_BASE       = (LPC_APB0_BASE + 0x20000);
constexpr intptr_t LPC_RTC_BASE       = (LPC_APB0_BASE + 0x24000);
constexpr intptr_t LPC_GPIOINT_BASE   = (LPC_APB0_BASE + 0x28080);
constexpr intptr_t LPC_PINCON_BASE    = (LPC_APB0_BASE + 0x2C000);
constexpr intptr_t LPC_SSP1_BASE      = (LPC_APB0_BASE + 0x30000);
constexpr intptr_t LPC_ADC_BASE       = (LPC_APB0_BASE + 0x34000);
constexpr intptr_t LPC_CANAF_RAM_BASE = (LPC_APB0_BASE + 0x38000);
constexpr intptr_t LPC_CANAF_BASE     = (LPC_APB0_BASE + 0x3C000);
constexpr intptr_t LPC_CANCR_BASE     = (LPC_APB0_BASE + 0x40000);
constexpr intptr_t LPC_CAN1_BASE      = (LPC_APB0_BASE + 0x44000);
constexpr intptr_t LPC_CAN2_BASE      = (LPC_APB0_BASE + 0x48000);
constexpr intptr_t LPC_I2C1_BASE      = (LPC_APB0_BASE + 0x5C000);

/* APB1 peripherals                                                           */
constexpr intptr_t LPC_SSP0_BASE  = (LPC_APB1_BASE + 0x08000);
constexpr intptr_t LPC_DAC_BASE   = (LPC_APB1_BASE + 0x0C000);
constexpr intptr_t LPC_TIM2_BASE  = (LPC_APB1_BASE + 0x10000);
constexpr intptr_t LPC_TIM3_BASE  = (LPC_APB1_BASE + 0x14000);
constexpr intptr_t LPC_UART2_BASE = (LPC_APB1_BASE + 0x18000);
constexpr intptr_t LPC_UART3_BASE = (LPC_APB1_BASE + 0x1C000);
constexpr intptr_t LPC_I2C2_BASE  = (LPC_APB1_BASE + 0x20000);
constexpr intptr_t LPC_I2S_BASE   = (LPC_APB1_BASE + 0x28000);
constexpr intptr_t LPC_RIT_BASE   = (LPC_APB1_BASE + 0x30000);
constexpr intptr_t LPC_MCPWM_BASE = (LPC_APB1_BASE + 0x38000);
constexpr intptr_t LPC_QEI_BASE   = (LPC_APB1_BASE + 0x3C000);
constexpr intptr_t LPC_SC_BASE    = (LPC_APB1_BASE + 0x7C000);

/* AHB peripherals                                                            */
constexpr intptr_t LPC_EMAC_BASE     = (LPC_AHB_BASE + 0x00000);
constexpr intptr_t LPC_GPDMA_BASE    = (LPC_AHB_BASE + 0x04000);
constexpr intptr_t LPC_GPDMACH0_BASE = (LPC_AHB_BASE + 0x04100);
constexpr intptr_t LPC_GPDMACH1_BASE = (LPC_AHB_BASE + 0x04120);
constexpr intptr_t LPC_GPDMACH2_BASE = (LPC_AHB_BASE + 0x04140);
constexpr intptr_t LPC_GPDMACH3_BASE = (LPC_AHB_BASE + 0x04160);
constexpr intptr_t LPC_GPDMACH4_BASE = (LPC_AHB_BASE + 0x04180);
constexpr intptr_t LPC_GPDMACH5_BASE = (LPC_AHB_BASE + 0x041A0);
constexpr intptr_t LPC_GPDMACH6_BASE = (LPC_AHB_BASE + 0x041C0);
constexpr intptr_t LPC_GPDMACH7_BASE = (LPC_AHB_BASE + 0x041E0);
constexpr intptr_t LPC_USB_BASE      = (LPC_AHB_BASE + 0x0C000);

/* GPIOs                                                                      */
constexpr intptr_t LPC_GPIO0_BASE = (LPC_GPIO_BASE + 0x00000);
constexpr intptr_t LPC_GPIO1_BASE = (LPC_GPIO_BASE + 0x00020);
constexpr intptr_t LPC_GPIO2_BASE = (LPC_GPIO_BASE + 0x00040);
constexpr intptr_t LPC_GPIO3_BASE = (LPC_GPIO_BASE + 0x00060);
constexpr intptr_t LPC_GPIO4_BASE = (LPC_GPIO_BASE + 0x00080);

/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
inline auto * const LPC_SC = reinterpret_cast<LPC_SC_TypeDef*>(LPC_SC_BASE);
inline auto * const LPC_GPIO0 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO0_BASE);
inline auto * const LPC_GPIO1 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO1_BASE);
inline auto * const LPC_GPIO2 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO2_BASE);
inline auto * const LPC_GPIO3 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO3_BASE);
inline auto * const LPC_GPIO4 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO4_BASE);
inline auto * const LPC_WDT = reinterpret_cast<LPC_WDT_TypeDef*>(LPC_WDT_BASE);
inline auto * const LPC_TIM0 = reinterpret_cast<LPC_TIM_TypeDef*>(LPC_TIM0_BASE);
inline auto * const LPC_TIM1 = reinterpret_cast<LPC_TIM_TypeDef*>(LPC_TIM1_BASE);
inline auto * const LPC_TIM2 = reinterpret_cast<LPC_TIM_TypeDef*>(LPC_TIM2_BASE);
inline auto * const LPC_TIM3 = reinterpret_cast<LPC_TIM_TypeDef*>(LPC_TIM3_BASE);
inline auto * const LPC_RIT = reinterpret_cast<LPC_RIT_TypeDef*>(LPC_RIT_BASE);
inline auto * const LPC_UART0 = reinterpret_cast<LPC_UART0_TypeDef*>(LPC_UART0_BASE);
inline auto * const LPC_UART1 = reinterpret_cast<LPC_UART1_TypeDef*>(LPC_UART1_BASE);
inline auto * const LPC_UART2 = reinterpret_cast<LPC_UART_TypeDef*>(LPC_UART2_BASE);
inline auto * const LPC_UART3 = reinterpret_cast<LPC_UART_TypeDef*>(LPC_UART3_BASE);
inline auto * const LPC_PWM1 = reinterpret_cast<LPC_PWM_TypeDef*>(LPC_PWM1_BASE);
inline auto * const LPC_I2C0 = reinterpret_cast<LPC_I2C_TypeDef*>(LPC_I2C0_BASE);
inline auto * const LPC_I2C1 = reinterpret_cast<LPC_I2C_TypeDef*>(LPC_I2C1_BASE);
inline auto * const LPC_I2C2 = reinterpret_cast<LPC_I2C_TypeDef*>(LPC_I2C2_BASE);
inline auto * const LPC_I2S = reinterpret_cast<LPC_I2S_TypeDef*>(LPC_I2S_BASE);
inline auto * const LPC_SPI = reinterpret_cast<LPC_SPI_TypeDef*>(LPC_SPI_BASE);
inline auto * const LPC_RTC = reinterpret_cast<LPC_RTC_TypeDef*>(LPC_RTC_BASE);
inline auto * const LPC_GPIOINT = reinterpret_cast<LPC_GPIOINT_TypeDef*>(LPC_GPIOINT_BASE);
inline auto * const LPC_PINCON = reinterpret_cast<LPC_PINCON_TypeDef*>(LPC_PINCON_BASE);
inline auto * const LPC_SSP0 = reinterpret_cast<LPC_SSP_TypeDef*>(LPC_SSP0_BASE);
inline auto * const LPC_SSP1 = reinterpret_cast<LPC_SSP_TypeDef*>(LPC_SSP1_BASE);
inline auto * const LPC_ADC = reinterpret_cast<LPC_ADC_TypeDef*>(LPC_ADC_BASE);
inline auto * const LPC_DAC = reinterpret_cast<LPC_DAC_TypeDef*>(LPC_DAC_BASE);
inline auto * const LPC_CANAF_RAM = reinterpret_cast<LPC_CANAF_RAM_TypeDef*>(LPC_CANAF_RAM_BASE);
inline auto * const LPC_CANAF = reinterpret_cast<LPC_CANAF_TypeDef*>(LPC_CANAF_BASE);
inline auto * const LPC_CANCR = reinterpret_cast<LPC_CANCR_TypeDef*>(LPC_CANCR_BASE);
inline auto * const LPC_CAN1 = reinterpret_cast<LPC_CAN_TypeDef*>(LPC_CAN1_BASE);
inline auto * const LPC_CAN2 = reinterpret_cast<LPC_CAN_TypeDef*>(LPC_CAN2_BASE);
inline auto * const LPC_MCPWM = reinterpret_cast<LPC_MCPWM_TypeDef*>(LPC_MCPWM_BASE);
inline auto * const LPC_QEI = reinterpret_cast<LPC_QEI_TypeDef*>(LPC_QEI_BASE);
inline auto * const LPC_EMAC = reinterpret_cast<LPC_EMAC_TypeDef*>(LPC_EMAC_BASE);
inline auto * const LPC_GPDMA = reinterpret_cast<LPC_GPDMA_TypeDef*>(LPC_GPDMA_BASE);
inline auto * const LPC_GPDMACH0 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH0_BASE);
inline auto * const LPC_GPDMACH1 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH1_BASE);
inline auto * const LPC_GPDMACH2 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH2_BASE);
inline auto * const LPC_GPDMACH3 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH3_BASE);
inline auto * const LPC_GPDMACH4 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH4_BASE);
inline auto * const LPC_GPDMACH5 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH5_BASE);
inline auto * const LPC_GPDMACH6 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH6_BASE);
inline auto * const LPC_GPDMACH7 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH7_BASE);
inline auto * const LPC_USB = reinterpret_cast<LPC_USB_TypeDef*>(LPC_USB_BASE);


#if defined (__cplusplus)
// SJSU-Dev2: Putting contents of this include in sjsu::lpc17xx
}  // namespace sjsu::lpc17xx
}  // extern "C"
#endif

#endif  // __LPC17xx_H__
