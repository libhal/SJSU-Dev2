/*************************************************************************
*
* $Id$		LPC407x_8x.h			2012-04-25
*
* @file     LPC407x_8x.h
*
* @brief    CMSIS Cortex-M4 Peripheral Access Layer Header File for
*           NXP LPC407x_8x.
* @version  V0.5
* @date     17. May 2012
* @author	NXP MCU SW Application Team
* @modified: August 9, 2019
*
* Copyright(C) 2012, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/

#ifndef __LPC40xx_H__
#define __LPC40xx_H__
#pragma GCC system_header


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the cm4 Processor and Core Peripherals---------------- */
#define __CM4_REV                 0x0000            /*!< Cortex-M4 Core Revision                                               */
#define __MPU_PRESENT                  1            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               3            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
#ifndef __FPU_PRESENT
    #define __FPU_PRESENT              1            /*!< FPU present or not                                                    */
#endif

// SJSU-Dev2: Moved these to the top so they are not included within the
// sjsu::lpc40xx namespace
/*!< Cortex-M4 processor and core peripherals                              */
#include "L0_Platform/arm_cortex/m4/core_cm4.h"

#if defined (__cplusplus)
// SJSU-Dev2: Putting contents of this include in sjsu::lpc40xx
namespace sjsu::lpc40xx
{
extern "C" {
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */
// SJSU-Dev2: Removed typedef of enum to IRQn_Type as IRQn_Type has been
// defined in core_cm4.h as an edit for this repository.
enum IRQn
{
/******  LPC177x_8x Specific Interrupt Numbers *******************************************************/
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
  Reserved0_IRQn                = 13,       /*!< Reserved                                         */
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
  MCI_IRQn                      = 29,       /*!< SD/MMC card I/F Interrupt                        */
  MCPWM_IRQn                    = 30,       /*!< Motor Control PWM Interrupt                      */
  QEI_IRQn                      = 31,       /*!< Quadrature Encoder Interface Interrupt           */
  PLL1_IRQn                     = 32,       /*!< PLL1 Lock (USB PLL) Interrupt                    */
  USBActivity_IRQn              = 33,       /*!< USB Activity interrupt                           */
  CANActivity_IRQn              = 34,       /*!< CAN Activity interrupt                           */
  UART4_IRQn                    = 35,       /*!< UART4 Interrupt                                  */
  SSP2_IRQn                     = 36,       /*!< SSP2 Interrupt                                   */
  LCD_IRQn                      = 37,       /*!< LCD Interrupt                                    */
  GPIO_IRQn                     = 38,       /*!< GPIO Interrupt                                   */
  PWM0_IRQn                     = 39,       /*!<  39  PWM0                                        */
  EEPROM_IRQn                   = 40,       /*!<  40  EEPROM                                      */
  CMP0_IRQn                     = 41,       /*!<  41  CMP0                                        */
  CMP1_IRQn                     = 42,       /*!<  42  CMP1                                        */
  kNumberOfIrqs
};
/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*------------- General Purpose Direct Memory Access (GPDMA) -----------------*/
typedef struct                          /* Common Registers                   */
{
  _I  uint32_t IntStat;
  _I  uint32_t IntTCStat;
  _O  uint32_t IntTCClear;
  _I  uint32_t IntErrStat;
  _O  uint32_t IntErrClr;
  _I  uint32_t RawIntTCStat;
  _I  uint32_t RawIntErrStat;
  _I  uint32_t EnbldChns;
  _IO uint32_t SoftBReq;
  _IO uint32_t SoftSReq;
  _IO uint32_t SoftLBReq;
  _IO uint32_t SoftLSReq;
  _IO uint32_t Config;
  _IO uint32_t Sync;
} LPC_GPDMA_TypeDef;

typedef struct                          /* Channel Registers                  */
{
  _IO uint32_t CSrcAddr;
  _IO uint32_t CDestAddr;
  _IO uint32_t CLLI;
  _IO uint32_t CControl;
  _IO uint32_t CConfig;
} LPC_GPDMACH_TypeDef;

/*------------- System Control (SC) ------------------------------------------*/
typedef struct
{
  _IO uint32_t FLASHCFG;                   /*!< Offset: 0x000 (R/W)  Flash Accelerator Configuration Register */
       uint32_t RESERVED0[31];
  _IO uint32_t PLL0CON;                    /*!< Offset: 0x080 (R/W)  PLL0 Control Register */
  _IO uint32_t PLL0CFG;                    /*!< Offset: 0x084 (R/W)  PLL0 Configuration Register */
  _I  uint32_t PLL0STAT;                   /*!< Offset: 0x088 (R/ )  PLL0 Status Register */
  _O  uint32_t PLL0FEED;                   /*!< Offset: 0x08C ( /W)  PLL0 Feed Register */
       uint32_t RESERVED1[4];
  _IO uint32_t PLL1CON;                    /*!< Offset: 0x0A0 (R/W)  PLL1 Control Register */
  _IO uint32_t PLL1CFG;                    /*!< Offset: 0x0A4 (R/W)  PLL1 Configuration Register */
  _I  uint32_t PLL1STAT;                   /*!< Offset: 0x0A8 (R/ )  PLL1 Status Register */
  _O  uint32_t PLL1FEED;                   /*!< Offset: 0x0AC ( /W)  PLL1 Feed Register */
       uint32_t RESERVED2[4];
  _IO uint32_t PCON;                       /*!< Offset: 0x0C0 (R/W)  Power Control Register */
  _IO uint32_t PCONP;                      /*!< Offset: 0x0C4 (R/W)  Power Control for Peripherals Register */
  _IO uint32_t PCONP1;                      /*!< Offset: 0x0C8 (R/W)  Power Control for Peripherals Register */
       uint32_t RESERVED3[13];
  _IO uint32_t EMCCLKSEL;                  /*!< Offset: 0x100 (R/W)  External Memory Controller Clock Selection Register */
  _IO uint32_t CCLKSEL;                    /*!< Offset: 0x104 (R/W)  CPU Clock Selection Register */
  _IO uint32_t USBCLKSEL;                  /*!< Offset: 0x108 (R/W)  USB Clock Selection Register */
  _IO uint32_t CLKSRCSEL;                  /*!< Offset: 0x10C (R/W)  Clock Source Select Register */
  _IO uint32_t	CANSLEEPCLR;                /*!< Offset: 0x110 (R/W)  CAN Sleep Clear Register */
  _IO uint32_t	CANWAKEFLAGS;               /*!< Offset: 0x114 (R/W)  CAN Wake-up Flags Register */
       uint32_t RESERVED4[10];
  _IO uint32_t EXTINT;                     /*!< Offset: 0x140 (R/W)  External Interrupt Flag Register */
       uint32_t RESERVED5[1];
  _IO uint32_t EXTMODE;                    /*!< Offset: 0x148 (R/W)  External Interrupt Mode Register */
  _IO uint32_t EXTPOLAR;                   /*!< Offset: 0x14C (R/W)  External Interrupt Polarity Register */
       uint32_t RESERVED6[12];
  _IO uint32_t RSID;                       /*!< Offset: 0x180 (R/W)  Reset Source Identification Register */
       uint32_t RESERVED7[7];
  _IO uint32_t SCS;                        /*!< Offset: 0x1A0 (R/W)  System Controls and Status Register */
  _IO uint32_t IRCTRIM;                    /*!< Offset: 0x1A4 (R/W) Clock Dividers                     */
  _IO uint32_t PCLKSEL;                    /*!< Offset: 0x1A8 (R/W)  Peripheral Clock Selection Register */
       uint32_t RESERVED8;
  _IO uint32_t PBOOST;						/*!< Offset: 0x1B0 (R/W)  Power Boost control register */
  _IO uint32_t SPIFISEL;
  _IO uint32_t LCD_CFG;                    /*!< Offset: 0x1B8 (R/W)  LCD Configuration and clocking control Register */
       uint32_t RESERVED10[1];
  _IO uint32_t USBIntSt;                   /*!< Offset: 0x1C0 (R/W)  USB Interrupt Status Register */
  _IO uint32_t DMAREQSEL;                  /*!< Offset: 0x1C4 (R/W)  DMA Request Select Register */
  _IO uint32_t CLKOUTCFG;                  /*!< Offset: 0x1C8 (R/W)  Clock Output Configuration Register */
  _IO uint32_t RSTCON0;                    /*!< Offset: 0x1CC (R/W)  RESET Control0 Register */
  _IO uint32_t RSTCON1;                    /*!< Offset: 0x1D0 (R/W)  RESET Control1 Register */
       uint32_t RESERVED11[2];
  _IO uint32_t EMCDLYCTL;                  /*!< Offset: 0x1DC (R/W) SDRAM programmable delays          */
  _IO uint32_t EMCCAL;                     /*!< Offset: 0x1E0 (R/W) Calibration of programmable delays */
 } LPC_SC_TypeDef;
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
  _I  uint32_t RxFilterWoLStatus;
  _O  uint32_t RxFilterWoLClear;
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

/*------------- LCD controller (LCD) -----------------------------------------*/
typedef struct
{
  _IO uint32_t TIMH;                   /* LCD Registers                      */
  _IO uint32_t TIMV;
  _IO uint32_t POL;
  _IO uint32_t LE;
  _IO uint32_t UPBASE;
  _IO uint32_t LPBASE;
  _IO uint32_t CTRL;
  _IO uint32_t INTMSK;
  _I  uint32_t INTRAW;
  _I  uint32_t INTSTAT;
  _O  uint32_t INTCLR;
  _I  uint32_t UPCURR;
  _I  uint32_t LPCURR;
       uint32_t RESERVED0[115];
  _IO uint32_t PAL[128];
       uint32_t RESERVED1[256];
  _IO uint32_t CRSR_IMG[256];
  _IO uint32_t CRSR_CTRL;
  _IO uint32_t CRSR_CFG;
  _IO uint32_t CRSR_PAL0;
  _IO uint32_t CRSR_PAL1;
  _IO uint32_t CRSR_XY;
  _IO uint32_t CRSR_CLIP;
       uint32_t RESERVED2[2];
  _IO uint32_t CRSR_INTMSK;
  _O  uint32_t CRSR_INTCLR;
  _I  uint32_t CRSR_INTRAW;
  _I  uint32_t CRSR_INTSTAT;
} LPC_LCD_TypeDef;

/*------------- Universal Serial Bus (USB) -----------------------------------*/
typedef struct
{
  _I  uint32_t Revision;             /* USB Host Registers                 */
  _IO uint32_t Control;
  _IO uint32_t CommandStatus;
  _IO uint32_t InterruptStatus;
  _IO uint32_t InterruptEnable;
  _IO uint32_t InterruptDisable;
  _IO uint32_t HCCA;
  _I  uint32_t PeriodCurrentED;
  _IO uint32_t ControlHeadED;
  _IO uint32_t ControlCurrentED;
  _IO uint32_t BulkHeadED;
  _IO uint32_t BulkCurrentED;
  _I  uint32_t DoneHead;
  _IO uint32_t FmInterval;
  _I  uint32_t FmRemaining;
  _I  uint32_t FmNumber;
  _IO uint32_t PeriodicStart;
  _IO uint32_t LSTreshold;
  _IO uint32_t RhDescriptorA;
  _IO uint32_t RhDescriptorB;
  _IO uint32_t RhStatus;
  _IO uint32_t RhPortStatus1;
  _IO uint32_t RhPortStatus2;
       uint32_t RESERVED0[40];
  _I  uint32_t Module_ID;

  _I  uint32_t IntSt;               /* USB On-The-Go Registers            */
  _IO uint32_t IntEn;
  _O  uint32_t IntSet;
  _O  uint32_t IntClr;
  _IO uint32_t StCtrl;
  _IO uint32_t Tmr;
       uint32_t RESERVED1[58];

  _I  uint32_t DevIntSt;            /* USB Device Interrupt Registers     */
  _IO uint32_t DevIntEn;
  _O  uint32_t DevIntClr;
  _O  uint32_t DevIntSet;

  _O  uint32_t CmdCode;             /* USB Device SIE Command Registers   */
  _I  uint32_t CmdData;

  _I  uint32_t RxData;              /* USB Device Transfer Registers      */
  _O  uint32_t TxData;
  _I  uint32_t RxPLen;
  _O  uint32_t TxPLen;
  _IO uint32_t Ctrl;
  _O  uint32_t DevIntPri;

  _I  uint32_t EpIntSt;             /* USB Device Endpoint Interrupt Regs */
  _IO uint32_t EpIntEn;
  _O  uint32_t EpIntClr;
  _O  uint32_t EpIntSet;
  _O  uint32_t EpIntPri;

  _IO uint32_t ReEp;                /* USB Device Endpoint Realization Reg*/
  _O  uint32_t EpInd;
  _IO uint32_t MaxPSize;

  _I  uint32_t DMARSt;              /* USB Device DMA Registers           */
  _O  uint32_t DMARClr;
  _O  uint32_t DMARSet;
       uint32_t RESERVED2[9];
  _IO uint32_t UDCAH;
  _I  uint32_t EpDMASt;
  _O  uint32_t EpDMAEn;
  _O  uint32_t EpDMADis;
  _I  uint32_t DMAIntSt;
  _IO uint32_t DMAIntEn;
       uint32_t RESERVED3[2];
  _I  uint32_t EoTIntSt;
  _O  uint32_t EoTIntClr;
  _O  uint32_t EoTIntSet;
  _I  uint32_t NDDRIntSt;
  _O  uint32_t NDDRIntClr;
  _O  uint32_t NDDRIntSet;
  _I  uint32_t SysErrIntSt;
  _O  uint32_t SysErrIntClr;
  _O  uint32_t SysErrIntSet;
       uint32_t RESERVED4[15];

  union {
  _I  uint32_t I2C_RX;                 /* USB OTG I2C Registers              */
  _O  uint32_t I2C_TX;
  };
  _IO  uint32_t I2C_STS;
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

/*------------- CRC Engine (CRC) -----------------------------------------*/
typedef struct
{
  _IO uint32_t MODE;
  _IO uint32_t SEED;
  union {
  _I  uint32_t SUM;
  struct {
  _O  uint32_t DATA;
  } WR_DATA_DWORD;

  struct {
  _O  uint16_t DATA;
       uint16_t RESERVED;
  }WR_DATA_WORD;

  struct {
  _O  uint8_t  DATA;
       uint8_t  RESERVED[3];
  	}WR_DATA_BYTE;
  };
} LPC_CRC_TypeDef;
/*------------- General Purpose Input/Output (GPIO) --------------------------*/
typedef struct
{
  _IO uint32_t DIR;
       uint32_t RESERVED0[3];
  _IO uint32_t MASK;
  _IO uint32_t PIN;
  _IO uint32_t SET;
  _O  uint32_t CLR;
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

/*------------- External Memory Controller (EMC) -----------------------------*/
typedef struct
{
  _IO uint32_t Control;
  _I  uint32_t Status;
  _IO uint32_t Config;
       uint32_t RESERVED0[5];
  _IO uint32_t DynamicControl;
  _IO uint32_t DynamicRefresh;
  _IO uint32_t DynamicReadConfig;
       uint32_t RESERVED1[1];
  _IO uint32_t DynamicRP;
  _IO uint32_t DynamicRAS;
  _IO uint32_t DynamicSREX;
  _IO uint32_t DynamicAPR;
  _IO uint32_t DynamicDAL;
  _IO uint32_t DynamicWR;
  _IO uint32_t DynamicRC;
  _IO uint32_t DynamicRFC;
  _IO uint32_t DynamicXSR;
  _IO uint32_t DynamicRRD;
  _IO uint32_t DynamicMRD;
       uint32_t RESERVED2[9];
  _IO uint32_t StaticExtendedWait;
       uint32_t RESERVED3[31];
  _IO uint32_t DynamicConfig0;
  _IO uint32_t DynamicRasCas0;
       uint32_t RESERVED4[6];
  _IO uint32_t DynamicConfig1;
  _IO uint32_t DynamicRasCas1;
       uint32_t RESERVED5[6];
  _IO uint32_t DynamicConfig2;
  _IO uint32_t DynamicRasCas2;
       uint32_t RESERVED6[6];
  _IO uint32_t DynamicConfig3;
  _IO uint32_t DynamicRasCas3;
       uint32_t RESERVED7[38];
  _IO uint32_t StaticConfig0;
  _IO uint32_t StaticWaitWen0;
  _IO uint32_t StaticWaitOen0;
  _IO uint32_t StaticWaitRd0;
  _IO uint32_t StaticWaitPage0;
  _IO uint32_t StaticWaitWr0;
  _IO uint32_t StaticWaitTurn0;
       uint32_t RESERVED8[1];
  _IO uint32_t StaticConfig1;
  _IO uint32_t StaticWaitWen1;
  _IO uint32_t StaticWaitOen1;
  _IO uint32_t StaticWaitRd1;
  _IO uint32_t StaticWaitPage1;
  _IO uint32_t StaticWaitWr1;
  _IO uint32_t StaticWaitTurn1;
       uint32_t RESERVED9[1];
  _IO uint32_t StaticConfig2;
  _IO uint32_t StaticWaitWen2;
  _IO uint32_t StaticWaitOen2;
  _IO uint32_t StaticWaitRd2;
  _IO uint32_t StaticWaitPage2;
  _IO uint32_t StaticWaitWr2;
  _IO uint32_t StaticWaitTurn2;
       uint32_t RESERVED10[1];
  _IO uint32_t StaticConfig3;
  _IO uint32_t StaticWaitWen3;
  _IO uint32_t StaticWaitOen3;
  _IO uint32_t StaticWaitRd3;
  _IO uint32_t StaticWaitPage3;
  _IO uint32_t StaticWaitWr3;
  _IO uint32_t StaticWaitTurn3;
} LPC_EMC_TypeDef;

/*------------- Watchdog Timer (WDT) -----------------------------------------*/
typedef struct
{
  _IO uint8_t  MOD;
       uint8_t  RESERVED0[3];
  _IO uint32_t TC;
  _O  uint8_t  FEED;
       uint8_t  RESERVED1[3];
  _I  uint32_t TV;
       uint32_t RESERVED2;
  _IO uint32_t WARNINT;
  _IO uint32_t WINDOW;
} LPC_WDT_TypeDef;

/*------------- Timer (TIM) --------------------------------------------------*/
typedef struct
{
  _IO uint32_t IR;                     /*!< Offset: 0x000 Interrupt Register (R/W) */
  _IO uint32_t TCR;                    /*!< Offset: 0x004 Timer Control Register (R/W) */
  _IO uint32_t TC;                     /*!< Offset: 0x008 Timer Counter Register (R/W) */
  _IO uint32_t PR;                     /*!< Offset: 0x00C Prescale Register (R/W) */
  _IO uint32_t PC;                     /*!< Offset: 0x010 Prescale Counter Register (R/W) */
  _IO uint32_t MCR;                    /*!< Offset: 0x014 Match Control Register (R/W) */
  _IO uint32_t MR0;                    /*!< Offset: 0x018 Match Register 0 (R/W) */
  _IO uint32_t MR1;                    /*!< Offset: 0x01C Match Register 1 (R/W) */
  _IO uint32_t MR2;                    /*!< Offset: 0x020 Match Register 2 (R/W) */
  _IO uint32_t MR3;                    /*!< Offset: 0x024 Match Register 3 (R/W) */
  _IO uint32_t CCR;                    /*!< Offset: 0x028 Capture Control Register (R/W) */
  _I  uint32_t CR0;                    /*!< Offset: 0x02C Capture Register 0 (R/ ) */
  _I  uint32_t CR1;					/*!< Offset: 0x030 Capture Register 1 (R/ ) */
       uint32_t RESERVED0[2];
  _IO uint32_t EMR;                    /*!< Offset: 0x03C External Match Register (R/W) */
       uint32_t RESERVED1[12];
  _IO uint32_t CTCR;                   /*!< Offset: 0x070 Count Control Register (R/W) */
} LPC_TIM_TypeDef;


/*------------- Pulse-Width Modulation (PWM) ---------------------------------*/
typedef struct
{
  _IO uint32_t IR;                     /*!< Offset: 0x000 Interrupt Register (R/W) */
  _IO uint32_t TCR;                    /*!< Offset: 0x004 Timer Control Register (R/W) */
  _IO uint32_t TC;                     /*!< Offset: 0x008 Timer Counter Register (R/W) */
  _IO uint32_t PR;                     /*!< Offset: 0x00C Prescale Register (R/W) */
  _IO uint32_t PC;                     /*!< Offset: 0x010 Prescale Counter Register (R/W) */
  _IO uint32_t MCR;                    /*!< Offset: 0x014 Match Control Register (R/W) */
  _IO uint32_t MR0;                    /*!< Offset: 0x018 Match Register 0 (R/W) */
  _IO uint32_t MR1;                    /*!< Offset: 0x01C Match Register 1 (R/W) */
  _IO uint32_t MR2;                    /*!< Offset: 0x020 Match Register 2 (R/W) */
  _IO uint32_t MR3;                    /*!< Offset: 0x024 Match Register 3 (R/W) */
  _IO uint32_t CCR;                    /*!< Offset: 0x028 Capture Control Register (R/W) */
  _I  uint32_t CR0;                    /*!< Offset: 0x02C Capture Register 0 (R/ ) */
  _I  uint32_t CR1;					/*!< Offset: 0x030 Capture Register 1 (R/ ) */
  _I  uint32_t CR2;					/*!< Offset: 0x034 Capture Register 2 (R/ ) */
  _I  uint32_t CR3;					/*!< Offset: 0x038 Capture Register 3 (R/ ) */
       uint32_t RESERVED0;
  _IO uint32_t MR4;					/*!< Offset: 0x040 Match Register 4 (R/W) */
  _IO uint32_t MR5;					/*!< Offset: 0x044 Match Register 5 (R/W) */
  _IO uint32_t MR6;					/*!< Offset: 0x048 Match Register 6 (R/W) */
  _IO uint32_t PCR;					/*!< Offset: 0x04C PWM Control Register (R/W) */
  _IO uint32_t LER;					/*!< Offset: 0x050 Load Enable Register (R/W) */
       uint32_t RESERVED1[7];
  _IO uint32_t CTCR;					/*!< Offset: 0x070 Counter Control Register (R/W) */
} LPC_PWM_TypeDef;

/*------------- Universal Asynchronous Receiver Transmitter (UARTx) -----------*/
/* There are three types of UARTs on the chip:
(1) UART0,UART2, and UART3 are the standard UART.
(2) UART1 is the standard with modem capability.
(3) USART(UART4) is the sync/async UART with smart card capability.
More details can be found on the Users Manual. */

#if 0
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
  _I  uint8_t  FIFOLVL;
} LPC_UART_TypeDef;
#else
typedef struct
{
	union
	{
		_I  uint8_t  RBR;
		_O  uint8_t  THR;
		_IO uint8_t  DLL;
		uint32_t RESERVED0;
	};
	union
	{
		_IO uint8_t  DLM;
		_IO uint32_t IER;
	};
	union
	{
		_I  uint32_t IIR;
		_O  uint8_t  FCR;
	};
	_IO uint8_t  LCR;
	uint8_t  RESERVED1[7];//Reserved
	_I  uint8_t  LSR;
	uint8_t  RESERVED2[7];//Reserved
	_IO uint8_t  SCR;
	uint8_t  RESERVED3[3];//Reserved
	_IO uint32_t ACR;
	_IO uint8_t  ICR;
	uint8_t  RESERVED4[3];//Reserved
	_IO uint8_t  FDR;
	uint8_t  RESERVED5[7];//Reserved
	_IO uint8_t  TER;
	uint8_t  RESERVED8[27];//Reserved
	_IO uint8_t  RS485CTRL;
	uint8_t  RESERVED9[3];//Reserved
	_IO uint8_t  ADRMATCH;
	uint8_t  RESERVED10[3];//Reserved
	_IO uint8_t  RS485DLY;
	uint8_t  RESERVED11[3];//Reserved
	_I  uint8_t  FIFOLVL;
}LPC_UART_TypeDef;
#endif


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
  _I  uint8_t  FIFOLVL;
} LPC_UART1_TypeDef;

typedef struct
{
  union {
  _I  uint32_t  RBR;                   /*!< Offset: 0x000 Receiver Buffer  Register (R/ ) */
  _O  uint32_t  THR;                   /*!< Offset: 0x000 Transmit Holding Register ( /W) */
  _IO uint32_t  DLL;                   /*!< Offset: 0x000 Divisor Latch LSB (R/W) */
  };
  union {
  _IO uint32_t  DLM;                   /*!< Offset: 0x004 Divisor Latch MSB (R/W) */
  _IO uint32_t  IER;                   /*!< Offset: 0x000 Interrupt Enable Register (R/W) */
  };
  union {
  _I  uint32_t  IIR;                   /*!< Offset: 0x008 Interrupt ID Register (R/ ) */
  _O  uint32_t  FCR;                   /*!< Offset: 0x008 FIFO Control Register ( /W) */
  };
  _IO uint32_t  LCR;                   /*!< Offset: 0x00C Line Control Register (R/W) */
  _IO uint32_t  MCR;                   /*!< Offset: 0x010 Modem control Register (R/W) */
  _I  uint32_t  LSR;                   /*!< Offset: 0x014 Line Status Register (R/ ) */
  _I  uint32_t  MSR;                   /*!< Offset: 0x018 Modem status Register (R/ ) */
  _IO uint32_t  SCR;                   /*!< Offset: 0x01C Scratch Pad Register (R/W) */
  _IO uint32_t  ACR;                   /*!< Offset: 0x020 Auto-baud Control Register (R/W) */
  _IO uint32_t  ICR;                   /*!< Offset: 0x024 irDA Control Register (R/W) */
  _IO uint32_t  FDR;                   /*!< Offset: 0x028 Fractional Divider Register (R/W) */
  _IO uint32_t  OSR;                   /*!< Offset: 0x02C Over sampling Register (R/W) */
  _O  uint32_t  POP;                   /*!< Offset: 0x030 NHP Pop Register (W) */
  _IO uint32_t  MODE;                  /*!< Offset: 0x034 NHP Mode selection Register (W) */
       uint32_t  RESERVED0[2];
  _IO uint32_t  HDEN;                  /*!< Offset: 0x040 Half duplex Enable Register (R/W) */
       uint32_t  RESERVED1;
  _IO uint32_t  SCI_CTRL;				/*!< Offset: 0x048 Smart card Interface Control Register (R/W) */
  _IO uint32_t  RS485CTRL;             /*!< Offset: 0x04C RS-485/EIA-485 Control Register (R/W) */
  _IO uint32_t  ADRMATCH;              /*!< Offset: 0x050 RS-485/EIA-485 address match Register (R/W) */
  _IO uint32_t  RS485DLY;              /*!< Offset: 0x054 RS-485/EIA-485 direction control delay Register (R/W) */
  _IO uint32_t  SYNCCTRL;              /*!< Offset: 0x058 Synchronous Mode Control Register (R/W ) */
  _IO uint32_t  TER;                   /*!< Offset: 0x05C Transmit Enable Register (R/W) */
       uint32_t  RESERVED2[989];
  _I  uint32_t  CFG;                   /*!< Offset: 0xFD4 Configuration Register (R) */
  _O  uint32_t  INTCE;                 /*!< Offset: 0xFD8 Interrupt Clear Enable Register (W) */
  _O  uint32_t  INTSE;                 /*!< Offset: 0xFDC Interrupt Set Enable Register (W) */
  _I  uint32_t  INTS;                  /*!< Offset: 0xFE0 Interrupt Status Register (R) */
  _I  uint32_t  INTE;                  /*!< Offset: 0xFE4 Interrupt Enable Register (R) */
  _O  uint32_t  INTCS;                 /*!< Offset: 0xFE8 Interrupt Clear Status Register (W) */
  _O  uint32_t  INTSS;                 /*!< Offset: 0xFEC Interrupt Set Status Register (W) */
       uint32_t  RESERVED3[3];
  _I  uint32_t  MID;                   /*!< Offset: 0xFFC Module Identification Register (R) */
} LPC_UART4_TypeDef;
/*------------- Inter-Integrated Circuit (I2C) -------------------------------*/
typedef struct
{
  _IO uint32_t CONSET;                 /*!< Offset: 0x000 I2C Control Set Register (R/W) */
  _I  uint32_t STAT;                   /*!< Offset: 0x004 I2C Status Register (R/ ) */
  _IO uint32_t DAT;                    /*!< Offset: 0x008 I2C Data Register (R/W) */
  _IO uint32_t ADR0;                   /*!< Offset: 0x00C I2C Slave Address Register 0 (R/W) */
  _IO uint32_t SCLH;                   /*!< Offset: 0x010 SCH Duty Cycle Register High Half Word (R/W) */
  _IO uint32_t SCLL;                   /*!< Offset: 0x014 SCL Duty Cycle Register Low Half Word (R/W) */
  _O  uint32_t CONCLR;                 /*!< Offset: 0x018 I2C Control Clear Register ( /W) */
  _IO uint32_t MMCTRL;                 /*!< Offset: 0x01C Monitor mode control register (R/W) */
  _IO uint32_t ADR1;                   /*!< Offset: 0x020 I2C Slave Address Register 1 (R/W) */
  _IO uint32_t ADR2;                   /*!< Offset: 0x024 I2C Slave Address Register 2 (R/W) */
  _IO uint32_t ADR3;                   /*!< Offset: 0x028 I2C Slave Address Register 3 (R/W) */
  _I  uint32_t DATA_BUFFER;            /*!< Offset: 0x02C Data buffer register ( /W) */
  _IO uint32_t MASK0;                  /*!< Offset: 0x030 I2C Slave address mask register 0 (R/W) */
  _IO uint32_t MASK1;                  /*!< Offset: 0x034 I2C Slave address mask register 1 (R/W) */
  _IO uint32_t MASK2;                  /*!< Offset: 0x038 I2C Slave address mask register 2 (R/W) */
  _IO uint32_t MASK3;                  /*!< Offset: 0x03C I2C Slave address mask register 3 (R/W) */
} LPC_I2C_TypeDef;

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
  _IO uint32_t ERSTATUS;
  _IO uint32_t ERCONTROL;
  _IO uint32_t ERCOUNTERS;
       uint32_t RESERVED22;
  _IO uint32_t ERFIRSTSTAMP0;
  _IO uint32_t ERFIRSTSTAMP1;
  _IO uint32_t ERFIRSTSTAMP2;
       uint32_t RESERVED23;
  _IO uint32_t ERLASTSTAMP0;
  _IO uint32_t ERLASTSTAMP1;
  _IO uint32_t ERLASTSTAMP2;
} LPC_RTC_TypeDef;

/*------------- Pin Connect Block (PINCON) -----------------------------------*/
typedef struct
{
  _IO uint32_t P0_0;				/* 0x000 */
  _IO uint32_t P0_1;
  _IO uint32_t P0_2;
  _IO uint32_t P0_3;
  _IO uint32_t P0_4;
  _IO uint32_t P0_5;
  _IO uint32_t P0_6;
  _IO uint32_t P0_7;

  _IO uint32_t P0_8;				/* 0x020 */
  _IO uint32_t P0_9;
  _IO uint32_t P0_10;
  _IO uint32_t P0_11;
  _IO uint32_t P0_12;
  _IO uint32_t P0_13;
  _IO uint32_t P0_14;
  _IO uint32_t P0_15;

  _IO uint32_t P0_16;				/* 0x040 */
  _IO uint32_t P0_17;
  _IO uint32_t P0_18;
  _IO uint32_t P0_19;
  _IO uint32_t P0_20;
  _IO uint32_t P0_21;
  _IO uint32_t P0_22;
  _IO uint32_t P0_23;

  _IO uint32_t P0_24;				/* 0x060 */
  _IO uint32_t P0_25;
  _IO uint32_t P0_26;
  _IO uint32_t P0_27;
  _IO uint32_t P0_28;
  _IO uint32_t P0_29;
  _IO uint32_t P0_30;
  _IO uint32_t P0_31;

  _IO uint32_t P1_0;				/* 0x080 */
  _IO uint32_t P1_1;
  _IO uint32_t P1_2;
  _IO uint32_t P1_3;
  _IO uint32_t P1_4;
  _IO uint32_t P1_5;
  _IO uint32_t P1_6;
  _IO uint32_t P1_7;

  _IO uint32_t P1_8;				/* 0x0A0 */
  _IO uint32_t P1_9;
  _IO uint32_t P1_10;
  _IO uint32_t P1_11;
  _IO uint32_t P1_12;
  _IO uint32_t P1_13;
  _IO uint32_t P1_14;
  _IO uint32_t P1_15;

  _IO uint32_t P1_16;				/* 0x0C0 */
  _IO uint32_t P1_17;
  _IO uint32_t P1_18;
  _IO uint32_t P1_19;
  _IO uint32_t P1_20;
  _IO uint32_t P1_21;
  _IO uint32_t P1_22;
  _IO uint32_t P1_23;

  _IO uint32_t P1_24;				/* 0x0E0 */
  _IO uint32_t P1_25;
  _IO uint32_t P1_26;
  _IO uint32_t P1_27;
  _IO uint32_t P1_28;
  _IO uint32_t P1_29;
  _IO uint32_t P1_30;
  _IO uint32_t P1_31;

  _IO uint32_t P2_0;				/* 0x100 */
  _IO uint32_t P2_1;
  _IO uint32_t P2_2;
  _IO uint32_t P2_3;
  _IO uint32_t P2_4;
  _IO uint32_t P2_5;
  _IO uint32_t P2_6;
  _IO uint32_t P2_7;

  _IO uint32_t P2_8;				/* 0x120 */
  _IO uint32_t P2_9;
  _IO uint32_t P2_10;
  _IO uint32_t P2_11;
  _IO uint32_t P2_12;
  _IO uint32_t P2_13;
  _IO uint32_t P2_14;
  _IO uint32_t P2_15;

  _IO uint32_t P2_16;				/* 0x140 */
  _IO uint32_t P2_17;
  _IO uint32_t P2_18;
  _IO uint32_t P2_19;
  _IO uint32_t P2_20;
  _IO uint32_t P2_21;
  _IO uint32_t P2_22;
  _IO uint32_t P2_23;

  _IO uint32_t P2_24;				/* 0x160 */
  _IO uint32_t P2_25;
  _IO uint32_t P2_26;
  _IO uint32_t P2_27;
  _IO uint32_t P2_28;
  _IO uint32_t P2_29;
  _IO uint32_t P2_30;
  _IO uint32_t P2_31;

  _IO uint32_t P3_0;				/* 0x180 */
  _IO uint32_t P3_1;
  _IO uint32_t P3_2;
  _IO uint32_t P3_3;
  _IO uint32_t P3_4;
  _IO uint32_t P3_5;
  _IO uint32_t P3_6;
  _IO uint32_t P3_7;

  _IO uint32_t P3_8;				/* 0x1A0 */
  _IO uint32_t P3_9;
  _IO uint32_t P3_10;
  _IO uint32_t P3_11;
  _IO uint32_t P3_12;
  _IO uint32_t P3_13;
  _IO uint32_t P3_14;
  _IO uint32_t P3_15;

  _IO uint32_t P3_16;				/* 0x1C0 */
  _IO uint32_t P3_17;
  _IO uint32_t P3_18;
  _IO uint32_t P3_19;
  _IO uint32_t P3_20;
  _IO uint32_t P3_21;
  _IO uint32_t P3_22;
  _IO uint32_t P3_23;

  _IO uint32_t P3_24;				/* 0x1E0 */
  _IO uint32_t P3_25;
  _IO uint32_t P3_26;
  _IO uint32_t P3_27;
  _IO uint32_t P3_28;
  _IO uint32_t P3_29;
  _IO uint32_t P3_30;
  _IO uint32_t P3_31;

  _IO uint32_t P4_0;				/* 0x200 */
  _IO uint32_t P4_1;
  _IO uint32_t P4_2;
  _IO uint32_t P4_3;
  _IO uint32_t P4_4;
  _IO uint32_t P4_5;
  _IO uint32_t P4_6;
  _IO uint32_t P4_7;

  _IO uint32_t P4_8;				/* 0x220 */
  _IO uint32_t P4_9;
  _IO uint32_t P4_10;
  _IO uint32_t P4_11;
  _IO uint32_t P4_12;
  _IO uint32_t P4_13;
  _IO uint32_t P4_14;
  _IO uint32_t P4_15;

  _IO uint32_t P4_16;				/* 0x240 */
  _IO uint32_t P4_17;
  _IO uint32_t P4_18;
  _IO uint32_t P4_19;
  _IO uint32_t P4_20;
  _IO uint32_t P4_21;
  _IO uint32_t P4_22;
  _IO uint32_t P4_23;

  _IO uint32_t P4_24;				/* 0x260 */
  _IO uint32_t P4_25;
  _IO uint32_t P4_26;
  _IO uint32_t P4_27;
  _IO uint32_t P4_28;
  _IO uint32_t P4_29;
  _IO uint32_t P4_30;
  _IO uint32_t P4_31;

  _IO uint32_t P5_0;				/* 0x280 */
  _IO uint32_t P5_1;
  _IO uint32_t P5_2;
  _IO uint32_t P5_3;
  _IO uint32_t P5_4;				/* 0x290 */
} LPC_IOCON_TypeDef;

/*------------- Synchronous Serial Communication (SSP) -----------------------*/
typedef struct
{
  _IO uint32_t CR0;                    /*!< Offset: 0x000 Control Register 0 (R/W) */
  _IO uint32_t CR1;                    /*!< Offset: 0x004 Control Register 1 (R/W) */
  _IO uint32_t DR;                     /*!< Offset: 0x008 Data Register (R/W) */
  _I  uint32_t SR;                     /*!< Offset: 0x00C Status Registe (R/ ) */
  _IO uint32_t CPSR;                   /*!< Offset: 0x010 Clock Prescale Register (R/W) */
  _IO uint32_t IMSC;                   /*!< Offset: 0x014 Interrupt Mask Set and Clear Register (R/W) */
  _IO uint32_t RIS;                    /*!< Offset: 0x018 Raw Interrupt Status Register (R/W) */
  _IO uint32_t MIS;                    /*!< Offset: 0x01C Masked Interrupt Status Register (R/W) */
  _IO uint32_t ICR;                    /*!< Offset: 0x020 SSPICR Interrupt Clear Register (R/W) */
  _IO uint32_t DMACR;
} LPC_SSP_TypeDef;

/*------------- Analog-to-Digital Converter (ADC) ----------------------------*/
typedef struct
{
  _IO uint32_t CR;                     /*!< Offset: 0x000       A/D Control Register (R/W) */
  _IO uint32_t GDR;                    /*!< Offset: 0x004       A/D Global Data Register (R/W) */
       uint32_t RESERVED0;
  _IO uint32_t INTEN;                  /*!< Offset: 0x00C       A/D Interrupt Enable Register (R/W) */
  _IO uint32_t DR[8];                  /*!< Offset: 0x010-0x02C A/D Channel 0..7 Data Register (R/W) */
  _I  uint32_t STAT;                   /*!< Offset: 0x030       A/D Status Register (R/ ) */
  _IO uint32_t ADTRM;
} LPC_ADC_TypeDef;

/*------------- Controller Area Network (CAN) --------------------------------*/
typedef struct
{
  _IO uint32_t mask[512];              /* ID Masks                           */
} LPC_CANAF_RAM_TypeDef;

typedef struct                          /* Acceptance Filter Registers        */
{
	///Offset: 0x00000000 - Acceptance Filter Register
	_IO uint32_t AFMR;

	///Offset: 0x00000004 - Standard Frame Individual Start Address Register
	_IO uint32_t SFF_sa;

	///Offset: 0x00000008 - Standard Frame Group Start Address Register
	_IO uint32_t SFF_GRP_sa;

	///Offset: 0x0000000C - Extended Frame Start Address Register
	_IO uint32_t EFF_sa;

	///Offset: 0x00000010 - Extended Frame Group Start Address Register
	_IO uint32_t EFF_GRP_sa;

	///Offset: 0x00000014 - End of AF Tables register
	_IO uint32_t ENDofTable;

	///Offset: 0x00000018 - LUT Error Address register
	_I  uint32_t LUTerrAd;

	///Offset: 0x0000001C - LUT Error Register
	_I  uint32_t LUTerr;

	///Offset: 0x00000020 - CAN Central Transmit Status Register
	_IO uint32_t FCANIE;

	///Offset: 0x00000024 - FullCAN Interrupt and Capture registers 0
	_IO uint32_t FCANIC0;

	///Offset: 0x00000028 - FullCAN Interrupt and Capture registers 1
	_IO uint32_t FCANIC1;
} LPC_CANAF_TypeDef;

typedef struct                          /* Central Registers                  */
{
  _I  uint32_t TxSR;
  _I  uint32_t RxSR;
  _I  uint32_t MSR;
} LPC_CANCR_TypeDef;

typedef struct                          /* Controller Registers               */
{
	///Offset: 0x00000000 - Controls the operating mode of the CAN Controller
	_IO uint32_t MOD;

	///Offset: 0x00000004 - Command bits that affect the state
	_O  uint32_t CMR;

	///Offset: 0x00000008 - Global Controller Status and Error Counters
	_IO uint32_t GSR;

	///Offset: 0x0000000C - Interrupt status, Arbitration Lost Capture, Error Code Capture
	_I  uint32_t ICR;

	///Offset: 0x00000010 - Interrupt Enable Register
	_IO uint32_t IER;

	///Offset: 0x00000014 - Bus Timing Register
	_IO uint32_t BTR;

	///Offset: 0x00000018 - Error Warning Limit
	_IO uint32_t EWL;

	///Offset: 0x0000001C - Status Register
	_I  uint32_t SR;

	///Offset: 0x00000020 - Receive frame status
	_IO uint32_t RFS;

	///Offset: 0x00000024 - Received Identifier
	_IO uint32_t RID;

	///Offset: 0x00000028 - Received data bytes 1-4
	_IO uint32_t RDA;

	///Offset: 0x0000002C - Received data bytes 5-8
	_IO uint32_t RDB;

	///Offset: 0x00000030 - Transmit frame info (Tx Buffer 1)
	_IO uint32_t TFI1;

	///Offset: 0x00000034 - Transmit Identifier (Tx Buffer 1)
	_IO uint32_t TID1;

	///Offset: 0x00000038 - Transmit data bytes 1-4 (Tx Buffer 1)
	_IO uint32_t TDA1;

	///Offset: 0x0000003C - Transmit data bytes 5-8 (Tx Buffer 1)
	_IO uint32_t TDB1;

	///Offset: 0x00000040 - Transmit frame info (Tx Buffer 2)
	_IO uint32_t TFI2;

	///Offset: 0x00000044 - Transmit Identifier (Tx Buffer 2)
	_IO uint32_t TID2;

	///Offset: 0x00000048 - Transmit data bytes 1-4 (Tx Buffer 2)
	_IO uint32_t TDA2;

	///Offset: 0x0000004C - Transmit data bytes 5-8 (Tx Buffer 2)
	_IO uint32_t TDB2;

	///Offset: 0x00000050 - Transmit frame info (Tx Buffer 3)
	_IO uint32_t TFI3;

	///Offset: 0x00000054 - Transmit Identifier (Tx Buffer 3)
	_IO uint32_t TID3;

	///Offset: 0x00000058 - Transmit data bytes 1-4 (Tx Buffer 3)
	_IO uint32_t TDA3;

	///Offset: 0x0000005C - Transmit data bytes 5-8 (Tx Buffer 3)
	_IO uint32_t TDB3;
} LPC_CAN_TypeDef;

/*------------- Digital-to-Analog Converter (DAC) ----------------------------*/
typedef struct
{
  _IO uint32_t CR;
  _IO uint32_t CTRL;
  _IO uint32_t CNTVAL;
} LPC_DAC_TypeDef;

/*------------- Inter IC Sound (I2S) -----------------------------------------*/
typedef struct
{
  _IO uint32_t DAO;
  _IO uint32_t DAI;
  _O  uint32_t TXFIFO;
  _I  uint32_t RXFIFO;
  _I  uint32_t STATE;
  _IO uint32_t DMA1;
  _IO uint32_t DMA2;
  _IO uint32_t IRQ;
  _IO uint32_t TXRATE;
  _IO uint32_t RXRATE;
  _IO uint32_t TXBITRATE;
  _IO uint32_t RXBITRATE;
  _IO uint32_t TXMODE;
  _IO uint32_t RXMODE;
} LPC_I2S_TypeDef;

/*------------- Motor Control Pulse-Width Modulation (MCPWM) -----------------*/
typedef struct
{
  _I  uint32_t CON;
  _O  uint32_t CON_SET;
  _O  uint32_t CON_CLR;
  _I  uint32_t CAPCON;
  _O  uint32_t CAPCON_SET;
  _O  uint32_t CAPCON_CLR;
  _IO uint32_t TC0;
  _IO uint32_t TC1;
  _IO uint32_t TC2;
  _IO uint32_t LIM0;
  _IO uint32_t LIM1;
  _IO uint32_t LIM2;
  _IO uint32_t MAT0;
  _IO uint32_t MAT1;
  _IO uint32_t MAT2;
  _IO uint32_t DT;
  _IO uint32_t CP;
  _IO uint32_t CAP0;
  _IO uint32_t CAP1;
  _IO uint32_t CAP2;
  _I  uint32_t INTEN;
  _O  uint32_t INTEN_SET;
  _O  uint32_t INTEN_CLR;
  _I  uint32_t CNTCON;
  _O  uint32_t CNTCON_SET;
  _O  uint32_t CNTCON_CLR;
  _I  uint32_t INTF;
  _O  uint32_t INTF_SET;
  _O  uint32_t INTF_CLR;
  _O  uint32_t CAP_CLR;
} LPC_MCPWM_TypeDef;

/*------------- Quadrature Encoder Interface (QEI) ---------------------------*/
typedef struct
{
  _O  uint32_t CON;
  _I  uint32_t STAT;
  _IO uint32_t CONF;
  _I  uint32_t POS;
  _IO uint32_t MAXPOS;
  _IO uint32_t CMPOS0;
  _IO uint32_t CMPOS1;
  _IO uint32_t CMPOS2;
  _I  uint32_t INXCNT;
  _IO uint32_t INXCMP0;
  _IO uint32_t LOAD;
  _I  uint32_t TIME;
  _I  uint32_t VEL;
  _I  uint32_t CAP;
  _IO uint32_t VELCOMP;
  _IO uint32_t FILTERPHA;
  _IO uint32_t FILTERPHB;
  _IO uint32_t FILTERINX;
  _IO uint32_t WINDOW;
  _IO uint32_t INXCMP1;
  _IO uint32_t INXCMP2;
       uint32_t RESERVED0[993];
  _O  uint32_t IEC;
  _O  uint32_t IES;
  _I  uint32_t INTSTAT;
  _I  uint32_t IE;
  _O  uint32_t CLR;
  _O  uint32_t SET;
} LPC_QEI_TypeDef;

/*------------- SD/MMC card Interface (MCI)-----------------------------------*/
typedef struct
{
  _IO uint32_t POWER;
  _IO uint32_t CLOCK;
  _IO uint32_t ARGUMENT;
  _IO uint32_t COMMAND;
  _I  uint32_t RESP_CMD;
  _I  uint32_t RESP0;
  _I  uint32_t RESP1;
  _I  uint32_t RESP2;
  _I  uint32_t RESP3;
  _IO uint32_t DATATMR;
  _IO uint32_t DATALEN;
  _IO uint32_t DATACTRL;
  _I  uint32_t DATACNT;
  _I  uint32_t STATUS;
  _O  uint32_t CLEAR;
  _IO uint32_t MASK0;
       uint32_t RESERVED0[2];
  _I  uint32_t FIFOCNT;
       uint32_t RESERVED1[13];
  _IO uint32_t FIFO[16];
} LPC_MCI_TypeDef;

/*------------- EEPROM Controller (EEPROM) -----------------------------------*/
typedef struct
{
  _IO uint32_t CMD;			/* 0x0080 */
  _IO uint32_t ADDR;
  _IO uint32_t WDATA;
  _IO uint32_t RDATA;
  _IO uint32_t WSTATE;			/* 0x0090 */
  _IO uint32_t CLKDIV;
  _IO uint32_t PWRDWN;			/* 0x0098 */
       uint32_t RESERVED0[975];
  _IO uint32_t INT_CLR_ENABLE;	/* 0x0FD8 */
  _IO uint32_t INT_SET_ENABLE;
  _IO uint32_t INT_STATUS;		/* 0x0FE0 */
  _IO uint32_t INT_ENABLE;
  _IO uint32_t INT_CLR_STATUS;
  _IO uint32_t INT_SET_STATUS;
} LPC_EEPROM_TypeDef;

/*------------- COMPARATOR ----------------------------------------------------*/

typedef struct {                                    /*!< (@ 0x40020000) COMPARATOR Structure                                   */
  _IO uint32_t  CTRL;                              /*!< (@ 0x40020000) Comparator block control register                      */
  _IO uint32_t  CTRL0;                             /*!< (@ 0x40020004) Comparator 0 control register                          */
  _IO uint32_t  CTRL1;                             /*!< (@ 0x40020008) Comparator 1 control register                          */
} LPC_COMPARATOR_Type;

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* Base addresses                                                             */
constexpr intptr_t LPC_FLASH_BASE    = 0x00000000UL;
constexpr intptr_t LPC_RAM_BASE      = 0x10000000UL;
constexpr intptr_t LPC_PERI_RAM_BASE = 0x20000000UL;
constexpr intptr_t LPC_APB0_BASE     = 0x40000000UL;
constexpr intptr_t LPC_APB1_BASE     = 0x40080000UL;
constexpr intptr_t LPC_AHBRAM1_BASE  = 0x20004000UL;
constexpr intptr_t LPC_AHB_BASE      = 0x20080000UL;
constexpr intptr_t LPC_CM3_BASE      = 0xE0000000UL;

/* APB0 peripherals                                                           */
constexpr intptr_t LPC_WDT_BASE        = LPC_APB0_BASE + 0x00000;
constexpr intptr_t LPC_TIM0_BASE       = LPC_APB0_BASE + 0x04000;
constexpr intptr_t LPC_TIM1_BASE       = LPC_APB0_BASE + 0x08000;
constexpr intptr_t LPC_UART0_BASE      = LPC_APB0_BASE + 0x0C000;
constexpr intptr_t LPC_UART1_BASE      = LPC_APB0_BASE + 0x10000;
constexpr intptr_t LPC_PWM0_BASE       = LPC_APB0_BASE + 0x14000;
constexpr intptr_t LPC_PWM1_BASE       = LPC_APB0_BASE + 0x18000;
constexpr intptr_t LPC_I2C0_BASE       = LPC_APB0_BASE + 0x1C000;
constexpr intptr_t LPC_COMPARATOR_BASE = LPC_APB0_BASE + 0x20000;
constexpr intptr_t LPC_RTC_BASE        = LPC_APB0_BASE + 0x24000;
constexpr intptr_t LPC_GPIOINT_BASE    = LPC_APB0_BASE + 0x28080;
constexpr intptr_t LPC_IOCON_BASE      = LPC_APB0_BASE + 0x2C000;
constexpr intptr_t LPC_SSP1_BASE       = LPC_APB0_BASE + 0x30000;
constexpr intptr_t LPC_ADC_BASE        = LPC_APB0_BASE + 0x34000;
constexpr intptr_t LPC_CANAF_RAM_BASE  = LPC_APB0_BASE + 0x38000;
constexpr intptr_t LPC_CANAF_BASE      = LPC_APB0_BASE + 0x3C000;
constexpr intptr_t LPC_CANCR_BASE      = LPC_APB0_BASE + 0x40000;
constexpr intptr_t LPC_CAN1_BASE       = LPC_APB0_BASE + 0x44000;
constexpr intptr_t LPC_CAN2_BASE       = LPC_APB0_BASE + 0x48000;
constexpr intptr_t LPC_I2C1_BASE       = LPC_APB0_BASE + 0x5C000;

/* APB1 peripherals                                                           */
constexpr intptr_t LPC_SSP0_BASE  = LPC_APB1_BASE + 0x08000;
constexpr intptr_t LPC_DAC_BASE   = LPC_APB1_BASE + 0x0C000;
constexpr intptr_t LPC_TIM2_BASE  = LPC_APB1_BASE + 0x10000;
constexpr intptr_t LPC_TIM3_BASE  = LPC_APB1_BASE + 0x14000;
constexpr intptr_t LPC_UART2_BASE = LPC_APB1_BASE + 0x18000;
constexpr intptr_t LPC_UART3_BASE = LPC_APB1_BASE + 0x1C000;
constexpr intptr_t LPC_I2C2_BASE  = LPC_APB1_BASE + 0x20000;
constexpr intptr_t LPC_UART4_BASE = LPC_APB1_BASE + 0x24000;
constexpr intptr_t LPC_I2S_BASE   = LPC_APB1_BASE + 0x28000;
constexpr intptr_t LPC_SSP2_BASE  = LPC_APB1_BASE + 0x2C000;
constexpr intptr_t LPC_MCPWM_BASE = LPC_APB1_BASE + 0x38000;
constexpr intptr_t LPC_QEI_BASE   = LPC_APB1_BASE + 0x3C000;
constexpr intptr_t LPC_MCI_BASE   = LPC_APB1_BASE + 0x40000;
constexpr intptr_t LPC_SC_BASE    = LPC_APB1_BASE + 0x7C000;

/* AHB peripherals                                                            */
constexpr intptr_t LPC_GPDMA_BASE    = LPC_AHB_BASE + 0x00000;
constexpr intptr_t LPC_GPDMACH0_BASE = LPC_AHB_BASE + 0x00100;
constexpr intptr_t LPC_GPDMACH1_BASE = LPC_AHB_BASE + 0x00120;
constexpr intptr_t LPC_GPDMACH2_BASE = LPC_AHB_BASE + 0x00140;
constexpr intptr_t LPC_GPDMACH3_BASE = LPC_AHB_BASE + 0x00160;
constexpr intptr_t LPC_GPDMACH4_BASE = LPC_AHB_BASE + 0x00180;
constexpr intptr_t LPC_GPDMACH5_BASE = LPC_AHB_BASE + 0x001A0;
constexpr intptr_t LPC_GPDMACH6_BASE = LPC_AHB_BASE + 0x001C0;
constexpr intptr_t LPC_GPDMACH7_BASE = LPC_AHB_BASE + 0x001E0;
constexpr intptr_t LPC_EMAC_BASE     = LPC_AHB_BASE + 0x04000;
constexpr intptr_t LPC_LCD_BASE      = LPC_AHB_BASE + 0x08000;
constexpr intptr_t LPC_USB_BASE      = LPC_AHB_BASE + 0x0C000;
constexpr intptr_t LPC_CRC_BASE      = LPC_AHB_BASE + 0x10000;
constexpr intptr_t LPC_GPIO0_BASE    = LPC_AHB_BASE + 0x18000;
constexpr intptr_t LPC_GPIO1_BASE    = LPC_AHB_BASE + 0x18020;
constexpr intptr_t LPC_GPIO2_BASE    = LPC_AHB_BASE + 0x18040;
constexpr intptr_t LPC_GPIO3_BASE    = LPC_AHB_BASE + 0x18060;
constexpr intptr_t LPC_GPIO4_BASE    = LPC_AHB_BASE + 0x18080;
constexpr intptr_t LPC_GPIO5_BASE    = LPC_AHB_BASE + 0x180A0;
constexpr intptr_t LPC_EMC_BASE      = LPC_AHB_BASE + 0x1C000;

constexpr intptr_t LPC_EEPROM_BASE = LPC_FLASH_BASE + 0x200080;

/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
auto * const LPC_SC = reinterpret_cast<LPC_SC_TypeDef*>(LPC_SC_BASE);
auto * const LPC_WDT = reinterpret_cast<LPC_WDT_TypeDef*>(LPC_WDT_BASE);
auto * const LPC_TIM0 = reinterpret_cast<LPC_TIM_TypeDef*>(LPC_TIM0_BASE);
auto * const LPC_TIM1 = reinterpret_cast<LPC_TIM_TypeDef*>(LPC_TIM1_BASE);
auto * const LPC_TIM2 = reinterpret_cast<LPC_TIM_TypeDef*>(LPC_TIM2_BASE);
auto * const LPC_TIM3 = reinterpret_cast<LPC_TIM_TypeDef*>(LPC_TIM3_BASE);
auto * const LPC_UART0 = reinterpret_cast<LPC_UART_TypeDef*>(LPC_UART0_BASE);
auto * const LPC_UART1 = reinterpret_cast<LPC_UART1_TypeDef*>(LPC_UART1_BASE);
auto * const LPC_UART2 = reinterpret_cast<LPC_UART_TypeDef*>(LPC_UART2_BASE);
auto * const LPC_UART3 = reinterpret_cast<LPC_UART_TypeDef*>(LPC_UART3_BASE);
auto * const LPC_UART4 = reinterpret_cast<LPC_UART4_TypeDef*>(LPC_UART4_BASE);
auto * const LPC_PWM0 = reinterpret_cast<LPC_PWM_TypeDef*>(LPC_PWM0_BASE);
auto * const LPC_PWM1 = reinterpret_cast<LPC_PWM_TypeDef*>(LPC_PWM1_BASE);
auto * const LPC_I2C0 = reinterpret_cast<LPC_I2C_TypeDef*>(LPC_I2C0_BASE);
auto * const LPC_I2C1 = reinterpret_cast<LPC_I2C_TypeDef*>(LPC_I2C1_BASE);
auto * const LPC_I2C2 = reinterpret_cast<LPC_I2C_TypeDef*>(LPC_I2C2_BASE);
auto * const LPC_I2S = reinterpret_cast<LPC_I2S_TypeDef*>(LPC_I2S_BASE);
auto * const LPC_COMPARATOR = reinterpret_cast<LPC_COMPARATOR_Type*>( LPC_COMPARATOR_BASE);
auto * const LPC_RTC = reinterpret_cast<LPC_RTC_TypeDef*>(LPC_RTC_BASE);
auto * const LPC_GPIOINT = reinterpret_cast<LPC_GPIOINT_TypeDef*>(LPC_GPIOINT_BASE);
auto * const LPC_IOCON = reinterpret_cast<LPC_IOCON_TypeDef*>(LPC_IOCON_BASE);
auto * const LPC_SSP0 = reinterpret_cast<LPC_SSP_TypeDef*>(LPC_SSP0_BASE);
auto * const LPC_SSP1 = reinterpret_cast<LPC_SSP_TypeDef*>(LPC_SSP1_BASE);
auto * const LPC_SSP2 = reinterpret_cast<LPC_SSP_TypeDef*>(LPC_SSP2_BASE);
auto * const LPC_ADC = reinterpret_cast<LPC_ADC_TypeDef*>(LPC_ADC_BASE);
auto * const LPC_DAC = reinterpret_cast<LPC_DAC_TypeDef*>(LPC_DAC_BASE);
auto * const LPC_CANAF_RAM = reinterpret_cast<LPC_CANAF_RAM_TypeDef*>(LPC_CANAF_RAM_BASE);
auto * const LPC_CANAF = reinterpret_cast<LPC_CANAF_TypeDef*>(LPC_CANAF_BASE);
auto * const LPC_CANCR = reinterpret_cast<LPC_CANCR_TypeDef*>(LPC_CANCR_BASE);
auto * const LPC_CAN1 = reinterpret_cast<LPC_CAN_TypeDef*>(LPC_CAN1_BASE);
auto * const LPC_CAN2 = reinterpret_cast<LPC_CAN_TypeDef*>(LPC_CAN2_BASE);
auto * const LPC_MCPWM = reinterpret_cast<LPC_MCPWM_TypeDef*>(LPC_MCPWM_BASE);
auto * const LPC_QEI = reinterpret_cast<LPC_QEI_TypeDef*>(LPC_QEI_BASE);
auto * const LPC_MCI = reinterpret_cast<LPC_MCI_TypeDef*>(LPC_MCI_BASE);
auto * const LPC_GPDMA = reinterpret_cast<LPC_GPDMA_TypeDef*>(LPC_GPDMA_BASE);
auto * const LPC_GPDMACH0 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH0_BASE);
auto * const LPC_GPDMACH1 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH1_BASE);
auto * const LPC_GPDMACH2 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH2_BASE);
auto * const LPC_GPDMACH3 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH3_BASE);
auto * const LPC_GPDMACH4 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH4_BASE);
auto * const LPC_GPDMACH5 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH5_BASE);
auto * const LPC_GPDMACH6 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH6_BASE);
auto * const LPC_GPDMACH7 = reinterpret_cast<LPC_GPDMACH_TypeDef*>(LPC_GPDMACH7_BASE);
auto * const LPC_EMAC = reinterpret_cast<LPC_EMAC_TypeDef*>(LPC_EMAC_BASE);
auto * const LPC_LCD = reinterpret_cast<LPC_LCD_TypeDef*>(LPC_LCD_BASE);
auto * const LPC_USB = reinterpret_cast<LPC_USB_TypeDef*>(LPC_USB_BASE);
auto * const LPC_GPIO0 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO0_BASE);
auto * const LPC_GPIO1 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO1_BASE);
auto * const LPC_GPIO2 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO2_BASE);
auto * const LPC_GPIO3 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO3_BASE);
auto * const LPC_GPIO4 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO4_BASE);
auto * const LPC_GPIO5 = reinterpret_cast<LPC_GPIO_TypeDef*>(LPC_GPIO5_BASE);
auto * const LPC_EMC = reinterpret_cast<LPC_EMC_TypeDef*>(LPC_EMC_BASE);
auto * const LPC_CRC = reinterpret_cast<LPC_CRC_TypeDef*>(LPC_CRC_BASE);
auto * const LPC_EEPROM = reinterpret_cast<LPC_EEPROM_TypeDef*>(LPC_EEPROM_BASE);

#if defined (__cplusplus)
} // extern "C"
} // namespace sjsu::lpc40xx
#endif

#endif  // __LPC40xx_H__