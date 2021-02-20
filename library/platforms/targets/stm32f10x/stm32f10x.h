#pragma once
/**
 ******************************************************************************
 * @file    stm32f10x.h
 * @author  MCD Application Team
 * @version V3.5.0
 * @date    11-March-2011
 * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer Header File.
 *          This file contains all the peripheral register's definitions, bits
 *          definitions and memory mapping for STM32F10x Connectivity line,
 *          High density, High density value line, Medium density,
 *          Medium density Value line, Low density, Low density Value line
 *          and XL-density devices.
 *
 *          The file is the unique include file that the application programmer
 *          is using in the C source code, usually in main.c. This file
 *contains:
 *           - Configuration section that allows to select:
 *              - The device used in the target application
 *              - To use or not the peripheral’s drivers in application
 *code(i.e. code will be based on direct access to peripheral’s registers rather
 *than drivers API), this option is controlled by
 *                "#define USE_STDPERIPH_DRIVER"
 *              - To change few application-specific parameters such as the HSE
 *                crystal frequency
 *           - Data structures and the address mapping for all peripherals
 *           - Peripheral's registers declarations and bits definition
 *           - Macros to access peripheral’s registers hardware
 *
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

#pragma GCC system_header

#include "platforms/processors/arm_cortex/m4/core_cm4.h"
#include <cstdint>

// SJSU-Dev2: Putting contents of this include in sjsu::stm32f10x
namespace sjsu::stm32f10x
{
/**
 * @brief Configuration of the Cortex-M3 Processor and Core Peripherals
 */
#ifdef STM32F10X_XL
#define __MPU_PRESENT 1 /*!< STM32 XL-density devices provide an MPU */
#else
#define __MPU_PRESENT 0 /*!< Other STM32 devices does not provide an MPU */
#endif                  /* STM32F10X_XL */
#undef __MPU_PRESENT
#undef __NVIC_PRIO_BITS
#define __NVIC_PRIO_BITS 4 /*!< STM32 uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig \
  0 /*!< Set to 1 if different SysTick Config is used */

/**
 * @brief STM32F10x Interrupt Number Definition, according to the selected
 * device in @ref Library_configuration_section
 */
enum IRQn
{
  /******  Cortex-M3 Processor Exceptions Numbers ******/
  NonMaskableInt_IRQn   = -14, /*!< 2 Non Maskable */
  MemoryManagement_IRQn = -12, /*!< 4 Memory Management */
  BusFault_IRQn         = -11, /*!< 5 Bus Fault */
  UsageFault_IRQn       = -10, /*!< 6 Usage Fault */
  SVCall_IRQn           = -5,  /*!< 11 SV Call  */
  DebugMonitor_IRQn     = -4,  /*!< 12 Debug Monitor  */
  PendSV_IRQn           = -2,  /*!< 14 Pend SV  */
  SysTick_IRQn          = -1,  /*!< 15 System Tick  */
  /******  STM32 specific Interrupt Numbers ******/
  WWDG_IRQn               = 0,  /*!< Window WatchDog */
  PVD_IRQn                = 1,  /*!< PVD through EXTI Line detection */
  TAMPER_IRQn             = 2,  /*!< Tamper */
  RTC_IRQn                = 3,  /*!< RTC */
  FLASH_IRQn              = 4,  /*!< FLASH */
  RCC_IRQn                = 5,  /*!< RCC */
  EXTI0_IRQn              = 6,  /*!< EXTI Line0 */
  EXTI1_IRQn              = 7,  /*!< EXTI Line1 */
  EXTI2_IRQn              = 8,  /*!< EXTI Line2 */
  EXTI3_IRQn              = 9,  /*!< EXTI Line3 */
  EXTI4_IRQn              = 10, /*!< EXTI Line4 */
  DMA1_Channel1_IRQn      = 11, /*!< DMA1 Channel 1 */
  DMA1_Channel2_IRQn      = 12, /*!< DMA1 Channel 2 */
  DMA1_Channel3_IRQn      = 13, /*!< DMA1 Channel 3 */
  DMA1_Channel4_IRQn      = 14, /*!< DMA1 Channel 4 */
  DMA1_Channel5_IRQn      = 15, /*!< DMA1 Channel 5 */
  DMA1_Channel6_IRQn      = 16, /*!< DMA1 Channel 6 */
  DMA1_Channel7_IRQn      = 17, /*!< DMA1 Channel 7 */
  ADC1_IRQn               = 18, /*!< ADC1 */
  ADC1_2_IRQn             = 18, /*!< ADC1 and ADC2 */
  USB_HP_CAN1_TX_IRQn     = 19, /*!< USB Device High Priority or CAN1 TX */
  CAN1_TX_IRQn            = 19, /*!< USB Device High Priority or CAN1 TX */
  USB_LP_CAN1_RX0_IRQn    = 20, /*!< USB Device Low Priority or CAN1 RX0 */
  CAN1_RX0_IRQn           = 20, /*!< USB Device Low Priority or CAN1 RX0 */
  CAN1_RX1_IRQn           = 21, /*!< CAN1 RX1 */
  CAN1_SCE_IRQn           = 22, /*!< CAN1 SCE */
  EXTI9_5_IRQn            = 23, /*!< External Line[9:5] */
  TIM1_BRK_IRQn           = 24, /*!< TIM1 Break */
  TIM1_BRK_TIM15_IRQn     = 24, /*!< TIM1 Break and TIM15 */
  TIM1_BRK_TIM9_IRQn      = 24, /*!< TIM1 Break and TIM9 */
  TIM1_UP_IRQn            = 25, /*!< TIM1 Update */
  TIM1_UP_TIM16_IRQn      = 25, /*!< TIM1 Update and TIM16 */
  TIM1_UP_TIM10_IRQn      = 25, /*!< TIM1 Update and TIM10 */
  TIM1_TRG_COM_IRQn       = 26, /*!< TIM1 Trigger and Commutation */
  TIM1_TRG_COM_TIM11_IRQn = 26, /*!< TIM1 Trigger and Commutation */
  TIM1_CC_IRQn            = 27, /*!< TIM1 Capture Compare */
  TIM2_IRQn               = 28, /*!< TIM2 */
  TIM3_IRQn               = 29, /*!< TIM3 */
  TIM4_IRQn               = 30, /*!< TIM4 */
  I2C1_EV_IRQn            = 31, /*!< I2C1 Event */
  I2C1_ER_IRQn            = 32, /*!< I2C1 Error */
  I2C2_EV_IRQn            = 33, /*!< I2C2 Event */
  I2C2_ER_IRQn            = 34, /*!< I2C2 Error */
  SPI1_IRQn               = 35, /*!< SPI1 */
  SPI2_IRQn               = 36, /*!< SPI2 */
  USART1_IRQn             = 37, /*!< USART1 */
  USART2_IRQn             = 38, /*!< USART2 */
  USART3_IRQn             = 39, /*!< USART3 */
  EXTI15_10_IRQn          = 40, /*!< External Line[15:10] */
  RTCAlarm_IRQn           = 41, /*!< RTC Alarm through EXTI Line */
  USBWakeUp_IRQn          = 42, /*!< USB Device WakeUp */
  CEC_IRQn                = 42, /*!< HDMI-CEC */
  OTG_FS_WKUP_IRQn        = 42, /*!< USB OTG FS WakeUp */
  TIM8_BRK_IRQn           = 43, /*!< TIM8 Break */
  TIM12_IRQn              = 43, /*!< TIM12 */
  TIM8_BRK_TIM12_IRQn     = 43, /*!< TIM8 Break and TIM12*/
  TIM8_UP_IRQn            = 44, /*!< TIM8 Update */
  TIM13_IRQn              = 44, /*!< TIM13 */
  TIM8_UP_TIM13_IRQn      = 44, /*!< TIM8 Update and TIM13 */
  TIM8_TRG_COM_IRQn       = 45, /*!< TIM8 Trigger and Commutation */
  TIM14_IRQn              = 45, /*!< TIM14 */
  TIM8_TRG_COM_TIM14_IRQn = 45, /*!< TIM8 Trigger and Commutation */
  TIM8_CC_IRQn            = 46, /*!< TIM8 Capture Compare */
  ADC3_IRQn               = 47, /*!< ADC3 */
  FSMC_IRQn               = 48, /*!< FSMC */
  SDIO_IRQn               = 49, /*!< SDIO */
  TIM5_IRQn               = 50, /*!< TIM5 */
  SPI3_IRQn               = 51, /*!< SPI3 */
  UART4_IRQn              = 52, /*!< UART4 */
  UART5_IRQn              = 53, /*!< UART5 */
  TIM6_DAC_IRQn           = 54, /*!< TIM6 and DAC underrun */
  TIM6_IRQn               = 54, /*!< TIM6 */
  TIM7_IRQn               = 55, /*!< TIM7 */
  DMA2_Channel1_IRQn      = 56, /*!< DMA2 Channel 1 */
  DMA2_Channel2_IRQn      = 57, /*!< DMA2 Channel 2 */
  DMA2_Channel3_IRQn      = 58, /*!< DMA2 Channel 3 */
  DMA2_Channel4_5_IRQn    = 59, /*!< DMA2 Channel 4 and Channel 5 */
  DMA2_Channel4_IRQn      = 59, /*!< DMA2 Channel 4 */
  DMA2_Channel5_IRQn      = 60, /*!< DMA2 Channel 5 */
  ETH_IRQn                = 61, /*!< Ethernet */
  ETH_WKUP_IRQn           = 62, /*!< Ethernet Wakeup through EXTI line */
  CAN2_TX_IRQn            = 63, /*!< CAN2 TX */
  CAN2_RX0_IRQn           = 64, /*!< CAN2 RX0 */
  CAN2_RX1_IRQn           = 65, /*!< CAN2 RX1 */
  CAN2_SCE_IRQn           = 66, /*!< CAN2 SCE */
  OTG_FS_IRQn             = 67, /*!< USB OTG FS */
};

/**
 * @}
 */

/** @addtogroup Exported_types
 * @{
 */

/*!< STM32F10x Standard Peripheral Library old types (maintained for legacy
 * purpose) */
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef const int32_t sc32; /*!< Read Only */
typedef const int16_t sc16; /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef _IO int32_t vs32;
typedef _IO int16_t vs16;
typedef _IO int8_t vs8;

typedef _I int32_t vsc32; /*!< Read Only */
typedef _I int16_t vsc16; /*!< Read Only */
typedef _I int8_t vsc8;   /*!< Read Only */

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef const uint32_t uc32; /*!< Read Only */
typedef const uint16_t uc16; /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef _IO uint32_t vu32;
typedef _IO uint16_t vu16;
typedef _IO uint8_t vu8;

typedef _I uint32_t vuc32; /*!< Read Only */
typedef _I uint16_t vuc16; /*!< Read Only */
typedef _I uint8_t vuc8;   /*!< Read Only */

typedef enum
{
  RESET = 0,
  SET   = !RESET
} FlagStatus,
    ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE  = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  ERROR   = 0,
  SUCCESS = !ERROR
} ErrorStatus;

/*!< STM32F10x Standard Peripheral Library old definitions (maintained for
 * legacy purpose) */
#define HSEStartUp_TimeOut HSE_STARTUP_TIMEOUT
#define HSE_Value HSE_VALUE
#define HSI_Value HSI_VALUE
/**
 * @}
 */

/** @addtogroup Peripheral_registers_structures
 * @{
 */

/**
 * @brief Analog to Digital Converter
 */

typedef struct
{
  _IO uint32_t SR;
  _IO uint32_t CR1;
  _IO uint32_t CR2;
  _IO uint32_t SMPR1;
  _IO uint32_t SMPR2;
  _IO uint32_t JOFR1;
  _IO uint32_t JOFR2;
  _IO uint32_t JOFR3;
  _IO uint32_t JOFR4;
  _IO uint32_t HTR;
  _IO uint32_t LTR;
  _IO uint32_t SQR1;
  _IO uint32_t SQR2;
  _IO uint32_t SQR3;
  _IO uint32_t JSQR;
  _IO uint32_t JDR1;
  _IO uint32_t JDR2;
  _IO uint32_t JDR3;
  _IO uint32_t JDR4;
  _IO uint32_t DR;
} ADC_TypeDef;

/**
 * @brief Backup Registers
 */

typedef struct
{
  uint32_t RESERVED0;
  _IO uint16_t DR1;
  uint16_t RESERVED1;
  _IO uint16_t DR2;
  uint16_t RESERVED2;
  _IO uint16_t DR3;
  uint16_t RESERVED3;
  _IO uint16_t DR4;
  uint16_t RESERVED4;
  _IO uint16_t DR5;
  uint16_t RESERVED5;
  _IO uint16_t DR6;
  uint16_t RESERVED6;
  _IO uint16_t DR7;
  uint16_t RESERVED7;
  _IO uint16_t DR8;
  uint16_t RESERVED8;
  _IO uint16_t DR9;
  uint16_t RESERVED9;
  _IO uint16_t DR10;
  uint16_t RESERVED10;
  _IO uint16_t RTCCR;
  uint16_t RESERVED11;
  _IO uint16_t CR;
  uint16_t RESERVED12;
  _IO uint16_t CSR;
  uint16_t RESERVED13[5];
  _IO uint16_t DR11;
  uint16_t RESERVED14;
  _IO uint16_t DR12;
  uint16_t RESERVED15;
  _IO uint16_t DR13;
  uint16_t RESERVED16;
  _IO uint16_t DR14;
  uint16_t RESERVED17;
  _IO uint16_t DR15;
  uint16_t RESERVED18;
  _IO uint16_t DR16;
  uint16_t RESERVED19;
  _IO uint16_t DR17;
  uint16_t RESERVED20;
  _IO uint16_t DR18;
  uint16_t RESERVED21;
  _IO uint16_t DR19;
  uint16_t RESERVED22;
  _IO uint16_t DR20;
  uint16_t RESERVED23;
  _IO uint16_t DR21;
  uint16_t RESERVED24;
  _IO uint16_t DR22;
  uint16_t RESERVED25;
  _IO uint16_t DR23;
  uint16_t RESERVED26;
  _IO uint16_t DR24;
  uint16_t RESERVED27;
  _IO uint16_t DR25;
  uint16_t RESERVED28;
  _IO uint16_t DR26;
  uint16_t RESERVED29;
  _IO uint16_t DR27;
  uint16_t RESERVED30;
  _IO uint16_t DR28;
  uint16_t RESERVED31;
  _IO uint16_t DR29;
  uint16_t RESERVED32;
  _IO uint16_t DR30;
  uint16_t RESERVED33;
  _IO uint16_t DR31;
  uint16_t RESERVED34;
  _IO uint16_t DR32;
  uint16_t RESERVED35;
  _IO uint16_t DR33;
  uint16_t RESERVED36;
  _IO uint16_t DR34;
  uint16_t RESERVED37;
  _IO uint16_t DR35;
  uint16_t RESERVED38;
  _IO uint16_t DR36;
  uint16_t RESERVED39;
  _IO uint16_t DR37;
  uint16_t RESERVED40;
  _IO uint16_t DR38;
  uint16_t RESERVED41;
  _IO uint16_t DR39;
  uint16_t RESERVED42;
  _IO uint16_t DR40;
  uint16_t RESERVED43;
  _IO uint16_t DR41;
  uint16_t RESERVED44;
  _IO uint16_t DR42;
  uint16_t RESERVED45;
} BKP_TypeDef;

/**
 * @brief Controller Area Network TxMailBox
 */

typedef struct
{
  _IO uint32_t TIR;
  _IO uint32_t TDTR;
  _IO uint32_t TDLR;
  _IO uint32_t TDHR;
} CAN_TxMailBox_TypeDef;

/**
 * @brief Controller Area Network FIFOMailBox
 */

typedef struct
{
  _IO uint32_t RIR;
  _IO uint32_t RDTR;
  _IO uint32_t RDLR;
  _IO uint32_t RDHR;
} CAN_FIFOMailBox_TypeDef;

/**
 * @brief Controller Area Network FilterRegister
 */

typedef struct
{
  _IO uint32_t FR1;
  _IO uint32_t FR2;
} CAN_FilterRegister_TypeDef;

/**
 * @brief Controller Area Network
 */

typedef struct
{
  _IO uint32_t MCR;
  _IO uint32_t MSR;
  _IO uint32_t TSR;
  _IO uint32_t RF0R;
  _IO uint32_t RF1R;
  _IO uint32_t IER;
  _IO uint32_t ESR;
  _IO uint32_t BTR;
  uint32_t RESERVED0[88];
  CAN_TxMailBox_TypeDef sTxMailBox[3];
  CAN_FIFOMailBox_TypeDef sFIFOMailBox[2];
  uint32_t RESERVED1[12];
  _IO uint32_t FMR;
  _IO uint32_t FM1R;
  uint32_t RESERVED2;
  _IO uint32_t FS1R;
  uint32_t RESERVED3;
  _IO uint32_t FFA1R;
  uint32_t RESERVED4;
  _IO uint32_t FA1R;
  uint32_t RESERVED5[8];
#ifndef STM32F10X_CL
  CAN_FilterRegister_TypeDef sFilterRegister[14];
#else
  CAN_FilterRegister_TypeDef sFilterRegister[28];
#endif /* STM32F10X_CL */
} CAN_TypeDef;

/**
 * @brief Consumer Electronics Control (CEC)
 */
typedef struct
{
  _IO uint32_t CFGR;
  _IO uint32_t OAR;
  _IO uint32_t PRES;
  _IO uint32_t ESR;
  _IO uint32_t CSR;
  _IO uint32_t TXD;
  _IO uint32_t RXD;
} CEC_TypeDef;

/**
 * @brief CRC calculation unit
 */

typedef struct
{
  _IO uint32_t DR;
  _IO uint8_t IDR;
  uint8_t RESERVED0;
  uint16_t RESERVED1;
  _IO uint32_t CR;
} CRC_TypeDef;

/**
 * @brief Digital to Analog Converter
 */

typedef struct
{
  _IO uint32_t CR;
  _IO uint32_t SWTRIGR;
  _IO uint32_t DHR12R1;
  _IO uint32_t DHR12L1;
  _IO uint32_t DHR8R1;
  _IO uint32_t DHR12R2;
  _IO uint32_t DHR12L2;
  _IO uint32_t DHR8R2;
  _IO uint32_t DHR12RD;
  _IO uint32_t DHR12LD;
  _IO uint32_t DHR8RD;
  _IO uint32_t DOR1;
  _IO uint32_t DOR2;
#if defined(STM32F10X_LD_VL) || defined(STM32F10X_MD_VL) || \
    defined(STM32F10X_HD_VL)
  _IO uint32_t SR;
#endif
} DAC_TypeDef;

/**
 * @brief Debug MCU
 */

typedef struct
{
  _IO uint32_t IDCODE;
  _IO uint32_t CR;
} DBGMCU_TypeDef;

/**
 * @brief DMA Controller
 */

typedef struct
{
  _IO uint32_t CCR;
  _IO uint32_t CNDTR;
  _IO uint32_t CPAR;
  _IO uint32_t CMAR;
} DMA_Channel_TypeDef;

typedef struct
{
  _IO uint32_t ISR;
  _IO uint32_t IFCR;
} DMA_TypeDef;

/**
 * @brief Ethernet MAC
 */

typedef struct
{
  _IO uint32_t MACCR;
  _IO uint32_t MACFFR;
  _IO uint32_t MACHTHR;
  _IO uint32_t MACHTLR;
  _IO uint32_t MACMIIAR;
  _IO uint32_t MACMIIDR;
  _IO uint32_t MACFCR;
  _IO uint32_t MACVLANTR; /*    8 */
  uint32_t RESERVED0[2];
  _IO uint32_t MACRWUFFR; /*   11 */
  _IO uint32_t MACPMTCSR;
  uint32_t RESERVED1[2];
  _IO uint32_t MACSR; /*   15 */
  _IO uint32_t MACIMR;
  _IO uint32_t MACA0HR;
  _IO uint32_t MACA0LR;
  _IO uint32_t MACA1HR;
  _IO uint32_t MACA1LR;
  _IO uint32_t MACA2HR;
  _IO uint32_t MACA2LR;
  _IO uint32_t MACA3HR;
  _IO uint32_t MACA3LR; /*   24 */
  uint32_t RESERVED2[40];
  _IO uint32_t MMCCR; /*   65 */
  _IO uint32_t MMCRIR;
  _IO uint32_t MMCTIR;
  _IO uint32_t MMCRIMR;
  _IO uint32_t MMCTIMR; /*   69 */
  uint32_t RESERVED3[14];
  _IO uint32_t MMCTGFSCCR; /*   84 */
  _IO uint32_t MMCTGFMSCCR;
  uint32_t RESERVED4[5];
  _IO uint32_t MMCTGFCR;
  uint32_t RESERVED5[10];
  _IO uint32_t MMCRFCECR;
  _IO uint32_t MMCRFAECR;
  uint32_t RESERVED6[10];
  _IO uint32_t MMCRGUFCR;
  uint32_t RESERVED7[334];
  _IO uint32_t PTPTSCR;
  _IO uint32_t PTPSSIR;
  _IO uint32_t PTPTSHR;
  _IO uint32_t PTPTSLR;
  _IO uint32_t PTPTSHUR;
  _IO uint32_t PTPTSLUR;
  _IO uint32_t PTPTSAR;
  _IO uint32_t PTPTTHR;
  _IO uint32_t PTPTTLR;
  uint32_t RESERVED8[567];
  _IO uint32_t DMABMR;
  _IO uint32_t DMATPDR;
  _IO uint32_t DMARPDR;
  _IO uint32_t DMARDLAR;
  _IO uint32_t DMATDLAR;
  _IO uint32_t DMASR;
  _IO uint32_t DMAOMR;
  _IO uint32_t DMAIER;
  _IO uint32_t DMAMFBOCR;
  uint32_t RESERVED9[9];
  _IO uint32_t DMACHTDR;
  _IO uint32_t DMACHRDR;
  _IO uint32_t DMACHTBAR;
  _IO uint32_t DMACHRBAR;
} ETH_TypeDef;

/**
 * @brief External Interrupt/Event Controller
 */

typedef struct
{
  _IO uint32_t IMR;
  _IO uint32_t EMR;
  _IO uint32_t RTSR;
  _IO uint32_t FTSR;
  _IO uint32_t SWIER;
  _IO uint32_t PR;
} EXTI_TypeDef;

/**
 * @brief FLASH Registers
 */

typedef struct
{
  _IO uint32_t ACR;
  _IO uint32_t KEYR;
  _IO uint32_t OPTKEYR;
  _IO uint32_t SR;
  _IO uint32_t CR;
  _IO uint32_t AR;
  _IO uint32_t RESERVED;
  _IO uint32_t OBR;
  _IO uint32_t WRPR;
#ifdef STM32F10X_XL
  uint32_t RESERVED1[8];
  _IO uint32_t KEYR2;
  uint32_t RESERVED2;
  _IO uint32_t SR2;
  _IO uint32_t CR2;
  _IO uint32_t AR2;
#endif /* STM32F10X_XL */
} FLASH_TypeDef;

/**
 * @brief Option Bytes Registers
 */

typedef struct
{
  _IO uint16_t RDP;
  _IO uint16_t USER;
  _IO uint16_t Data0;
  _IO uint16_t Data1;
  _IO uint16_t WRP0;
  _IO uint16_t WRP1;
  _IO uint16_t WRP2;
  _IO uint16_t WRP3;
} OB_TypeDef;

/**
 * @brief Flexible Static Memory Controller
 */

typedef struct
{
  _IO uint32_t BTCR[8];
} FSMC_Bank1_TypeDef;

/**
 * @brief Flexible Static Memory Controller Bank1E
 */

typedef struct
{
  _IO uint32_t BWTR[7];
} FSMC_Bank1E_TypeDef;

/**
 * @brief Flexible Static Memory Controller Bank2
 */

typedef struct
{
  _IO uint32_t PCR2;
  _IO uint32_t SR2;
  _IO uint32_t PMEM2;
  _IO uint32_t PATT2;
  uint32_t RESERVED0;
  _IO uint32_t ECCR2;
} FSMC_Bank2_TypeDef;

/**
 * @brief Flexible Static Memory Controller Bank3
 */

typedef struct
{
  _IO uint32_t PCR3;
  _IO uint32_t SR3;
  _IO uint32_t PMEM3;
  _IO uint32_t PATT3;
  uint32_t RESERVED0;
  _IO uint32_t ECCR3;
} FSMC_Bank3_TypeDef;

/**
 * @brief Flexible Static Memory Controller Bank4
 */

typedef struct
{
  _IO uint32_t PCR4;
  _IO uint32_t SR4;
  _IO uint32_t PMEM4;
  _IO uint32_t PATT4;
  _IO uint32_t PIO4;
} FSMC_Bank4_TypeDef;

/**
 * @brief General Purpose I/O
 */

typedef struct
{
  _IO uint32_t CRL;
  _IO uint32_t CRH;
  _IO uint32_t IDR;
  _IO uint32_t ODR;
  _IO uint32_t BSRR;
  _IO uint32_t BRR;
  _IO uint32_t LCKR;
} GPIO_TypeDef;

/**
 * @brief Alternate Function I/O
 */

typedef struct
{
  _IO uint32_t EVCR;
  _IO uint32_t MAPR;
  _IO uint32_t EXTICR[4];
  uint32_t RESERVED0;
  _IO uint32_t MAPR2;
} AFIO_TypeDef;
/**
 * @brief Inter Integrated Circuit Interface
 */

typedef struct
{
  _IO uint16_t CR1;
  uint16_t RESERVED0;
  _IO uint16_t CR2;
  uint16_t RESERVED1;
  _IO uint16_t OAR1;
  uint16_t RESERVED2;
  _IO uint16_t OAR2;
  uint16_t RESERVED3;
  _IO uint16_t DR;
  uint16_t RESERVED4;
  _IO uint16_t SR1;
  uint16_t RESERVED5;
  _IO uint16_t SR2;
  uint16_t RESERVED6;
  _IO uint16_t CCR;
  uint16_t RESERVED7;
  _IO uint16_t TRISE;
  uint16_t RESERVED8;
} I2C_TypeDef;

/**
 * @brief Independent WATCHDOG
 */

typedef struct
{
  _IO uint32_t KR;
  _IO uint32_t PR;
  _IO uint32_t RLR;
  _IO uint32_t SR;
} IWDG_TypeDef;

/**
 * @brief Power Control
 */

typedef struct
{
  _IO uint32_t CR;
  _IO uint32_t CSR;
} PWR_TypeDef;

/**
 * @brief Reset and Clock Control
 */

typedef struct
{
  _IO uint32_t CR;
  _IO uint32_t CFGR;
  _IO uint32_t CIR;
  _IO uint32_t APB2RSTR;
  _IO uint32_t APB1RSTR;
  _IO uint32_t AHBENR;
  _IO uint32_t APB2ENR;
  _IO uint32_t APB1ENR;
  _IO uint32_t BDCR;
  _IO uint32_t CSR;

#ifdef STM32F10X_CL
  _IO uint32_t AHBRSTR;
  _IO uint32_t CFGR2;
#endif /* STM32F10X_CL */

#if defined(STM32F10X_LD_VL) || defined(STM32F10X_MD_VL) || \
    defined(STM32F10X_HD_VL)
  uint32_t RESERVED0;
  _IO uint32_t CFGR2;
#endif /* STM32F10X_LD_VL || STM32F10X_MD_VL || STM32F10X_HD_VL */
} RCC_TypeDef;

/**
 * @brief Real-Time Clock
 */

typedef struct
{
  _IO uint16_t CRH;
  uint16_t RESERVED0;
  _IO uint16_t CRL;
  uint16_t RESERVED1;
  _IO uint16_t PRLH;
  uint16_t RESERVED2;
  _IO uint16_t PRLL;
  uint16_t RESERVED3;
  _IO uint16_t DIVH;
  uint16_t RESERVED4;
  _IO uint16_t DIVL;
  uint16_t RESERVED5;
  _IO uint16_t CNTH;
  uint16_t RESERVED6;
  _IO uint16_t CNTL;
  uint16_t RESERVED7;
  _IO uint16_t ALRH;
  uint16_t RESERVED8;
  _IO uint16_t ALRL;
  uint16_t RESERVED9;
} RTC_TypeDef;

/**
 * @brief SD host Interface
 */

typedef struct
{
  _IO uint32_t POWER;
  _IO uint32_t CLKCR;
  _IO uint32_t ARG;
  _IO uint32_t CMD;
  _I uint32_t RESPCMD;
  _I uint32_t RESP1;
  _I uint32_t RESP2;
  _I uint32_t RESP3;
  _I uint32_t RESP4;
  _IO uint32_t DTIMER;
  _IO uint32_t DLEN;
  _IO uint32_t DCTRL;
  _I uint32_t DCOUNT;
  _I uint32_t STA;
  _IO uint32_t ICR;
  _IO uint32_t MASK;
  uint32_t RESERVED0[2];
  _I uint32_t FIFOCNT;
  uint32_t RESERVED1[13];
  _IO uint32_t FIFO;
} SDIO_TypeDef;

/**
 * @brief Serial Peripheral Interface
 */

typedef struct
{
  _IO uint16_t CR1;
  uint16_t RESERVED0;
  _IO uint16_t CR2;
  uint16_t RESERVED1;
  _IO uint16_t SR;
  uint16_t RESERVED2;
  _IO uint16_t DR;
  uint16_t RESERVED3;
  _IO uint16_t CRCPR;
  uint16_t RESERVED4;
  _IO uint16_t RXCRCR;
  uint16_t RESERVED5;
  _IO uint16_t TXCRCR;
  uint16_t RESERVED6;
  _IO uint16_t I2SCFGR;
  uint16_t RESERVED7;
  _IO uint16_t I2SPR;
  uint16_t RESERVED8;
} SPI_TypeDef;

/**
 * @brief TIM
 */

typedef struct
{
  _IO uint16_t CR1;
  uint16_t RESERVED0;
  _IO uint16_t CR2;
  uint16_t RESERVED1;
  _IO uint16_t SMCR;
  uint16_t RESERVED2;
  _IO uint16_t DIER;
  uint16_t RESERVED3;
  _IO uint16_t SR;
  uint16_t RESERVED4;
  _IO uint16_t EGR;
  uint16_t RESERVED5;
  _IO uint16_t CCMR1;
  uint16_t RESERVED6;
  _IO uint16_t CCMR2;
  uint16_t RESERVED7;
  _IO uint16_t CCER;
  uint16_t RESERVED8;
  _IO uint16_t CNT;
  uint16_t RESERVED9;
  _IO uint16_t PSC;
  uint16_t RESERVED10;
  _IO uint16_t ARR;
  uint16_t RESERVED11;
  _IO uint16_t RCR;
  uint16_t RESERVED12;
  _IO uint16_t CCR1;
  uint16_t RESERVED13;
  _IO uint16_t CCR2;
  uint16_t RESERVED14;
  _IO uint16_t CCR3;
  uint16_t RESERVED15;
  _IO uint16_t CCR4;
  uint16_t RESERVED16;
  _IO uint16_t BDTR;
  uint16_t RESERVED17;
  _IO uint16_t DCR;
  uint16_t RESERVED18;
  _IO uint16_t DMAR;
  uint16_t RESERVED19;
} TIM_TypeDef;

/**
 * @brief Universal Synchronous Asynchronous Receiver Transmitter
 */

typedef struct
{
  _IO uint16_t SR;
  uint16_t RESERVED0;
  _IO uint16_t DR;
  uint16_t RESERVED1;
  _IO uint16_t BRR;
  uint16_t RESERVED2;
  _IO uint16_t CR1;
  uint16_t RESERVED3;
  _IO uint16_t CR2;
  uint16_t RESERVED4;
  _IO uint16_t CR3;
  uint16_t RESERVED5;
  _IO uint16_t GTPR;
  uint16_t RESERVED6;
} USART_TypeDef;

/**
 * @brief Window WATCHDOG
 */

typedef struct
{
  _IO uint32_t CR;
  _IO uint32_t CFR;
  _IO uint32_t SR;
} WWDG_TypeDef;

/**
 * @}
 */

/** @addtogroup Peripheral_memory_map
 * @{
 */

/*!< FLASH base address in the alias region */
constexpr intptr_t FLASH_BASE = 0x08000000;
/*!< SRAM base address in the alias region */
constexpr intptr_t SRAM_BASE = 0x20000000;
/*!< Peripheral base address in the alias region */
constexpr intptr_t PERIPH_BASE = 0x40000000;
/*!< SRAM base address in the bit-band region */
constexpr intptr_t SRAM_BB_BASE = 0x22000000;

/*!< Peripheral base address in the bit-band region */
constexpr intptr_t PERIPH_BB_BASE = 0x42000000;
/*!< FSMC registers base address */
constexpr intptr_t FSMC_R_BASE = 0xA0000000;

/*!< Peripheral memory map */
constexpr intptr_t APB1PERIPH_BASE = PERIPH_BASE;
constexpr intptr_t APB2PERIPH_BASE = (PERIPH_BASE + 0x10000);
constexpr intptr_t AHBPERIPH_BASE  = (PERIPH_BASE + 0x20000);

constexpr intptr_t TIM2_BASE   = (APB1PERIPH_BASE + 0x0000);
constexpr intptr_t TIM3_BASE   = (APB1PERIPH_BASE + 0x0400);
constexpr intptr_t TIM4_BASE   = (APB1PERIPH_BASE + 0x0800);
constexpr intptr_t TIM5_BASE   = (APB1PERIPH_BASE + 0x0C00);
constexpr intptr_t TIM6_BASE   = (APB1PERIPH_BASE + 0x1000);
constexpr intptr_t TIM7_BASE   = (APB1PERIPH_BASE + 0x1400);
constexpr intptr_t TIM12_BASE  = (APB1PERIPH_BASE + 0x1800);
constexpr intptr_t TIM13_BASE  = (APB1PERIPH_BASE + 0x1C00);
constexpr intptr_t TIM14_BASE  = (APB1PERIPH_BASE + 0x2000);
constexpr intptr_t RTC_BASE    = (APB1PERIPH_BASE + 0x2800);
constexpr intptr_t WWDG_BASE   = (APB1PERIPH_BASE + 0x2C00);
constexpr intptr_t IWDG_BASE   = (APB1PERIPH_BASE + 0x3000);
constexpr intptr_t SPI2_BASE   = (APB1PERIPH_BASE + 0x3800);
constexpr intptr_t SPI3_BASE   = (APB1PERIPH_BASE + 0x3C00);
constexpr intptr_t USART2_BASE = (APB1PERIPH_BASE + 0x4400);
constexpr intptr_t USART3_BASE = (APB1PERIPH_BASE + 0x4800);
constexpr intptr_t UART4_BASE  = (APB1PERIPH_BASE + 0x4C00);
constexpr intptr_t UART5_BASE  = (APB1PERIPH_BASE + 0x5000);
constexpr intptr_t I2C1_BASE   = (APB1PERIPH_BASE + 0x5400);
constexpr intptr_t I2C2_BASE   = (APB1PERIPH_BASE + 0x5800);
constexpr intptr_t CAN1_BASE   = (APB1PERIPH_BASE + 0x6400);
constexpr intptr_t CAN2_BASE   = (APB1PERIPH_BASE + 0x6800);
constexpr intptr_t BKP_BASE    = (APB1PERIPH_BASE + 0x6C00);
constexpr intptr_t PWR_BASE    = (APB1PERIPH_BASE + 0x7000);
constexpr intptr_t DAC_BASE    = (APB1PERIPH_BASE + 0x7400);
constexpr intptr_t CEC_BASE    = (APB1PERIPH_BASE + 0x7800);

constexpr intptr_t AFIO_BASE   = (APB2PERIPH_BASE + 0x0000);
constexpr intptr_t EXTI_BASE   = (APB2PERIPH_BASE + 0x0400);
constexpr intptr_t GPIOA_BASE  = (APB2PERIPH_BASE + 0x0800);
constexpr intptr_t GPIOB_BASE  = (APB2PERIPH_BASE + 0x0C00);
constexpr intptr_t GPIOC_BASE  = (APB2PERIPH_BASE + 0x1000);
constexpr intptr_t GPIOD_BASE  = (APB2PERIPH_BASE + 0x1400);
constexpr intptr_t GPIOE_BASE  = (APB2PERIPH_BASE + 0x1800);
constexpr intptr_t GPIOF_BASE  = (APB2PERIPH_BASE + 0x1C00);
constexpr intptr_t GPIOG_BASE  = (APB2PERIPH_BASE + 0x2000);
constexpr intptr_t ADC1_BASE   = (APB2PERIPH_BASE + 0x2400);
constexpr intptr_t ADC2_BASE   = (APB2PERIPH_BASE + 0x2800);
constexpr intptr_t TIM1_BASE   = (APB2PERIPH_BASE + 0x2C00);
constexpr intptr_t SPI1_BASE   = (APB2PERIPH_BASE + 0x3000);
constexpr intptr_t TIM8_BASE   = (APB2PERIPH_BASE + 0x3400);
constexpr intptr_t USART1_BASE = (APB2PERIPH_BASE + 0x3800);
constexpr intptr_t ADC3_BASE   = (APB2PERIPH_BASE + 0x3C00);
constexpr intptr_t TIM15_BASE  = (APB2PERIPH_BASE + 0x4000);
constexpr intptr_t TIM16_BASE  = (APB2PERIPH_BASE + 0x4400);
constexpr intptr_t TIM17_BASE  = (APB2PERIPH_BASE + 0x4800);
constexpr intptr_t TIM9_BASE   = (APB2PERIPH_BASE + 0x4C00);
constexpr intptr_t TIM10_BASE  = (APB2PERIPH_BASE + 0x5000);
constexpr intptr_t TIM11_BASE  = (APB2PERIPH_BASE + 0x5400);

constexpr intptr_t SDIO_BASE = (PERIPH_BASE + 0x18000);

constexpr intptr_t DMA1_BASE          = (AHBPERIPH_BASE + 0x0000);
constexpr intptr_t DMA1_Channel1_BASE = (AHBPERIPH_BASE + 0x0008);
constexpr intptr_t DMA1_Channel2_BASE = (AHBPERIPH_BASE + 0x001C);
constexpr intptr_t DMA1_Channel3_BASE = (AHBPERIPH_BASE + 0x0030);
constexpr intptr_t DMA1_Channel4_BASE = (AHBPERIPH_BASE + 0x0044);
constexpr intptr_t DMA1_Channel5_BASE = (AHBPERIPH_BASE + 0x0058);
constexpr intptr_t DMA1_Channel6_BASE = (AHBPERIPH_BASE + 0x006C);
constexpr intptr_t DMA1_Channel7_BASE = (AHBPERIPH_BASE + 0x0080);
constexpr intptr_t DMA2_BASE          = (AHBPERIPH_BASE + 0x0400);
constexpr intptr_t DMA2_Channel1_BASE = (AHBPERIPH_BASE + 0x0408);
constexpr intptr_t DMA2_Channel2_BASE = (AHBPERIPH_BASE + 0x041C);
constexpr intptr_t DMA2_Channel3_BASE = (AHBPERIPH_BASE + 0x0430);
constexpr intptr_t DMA2_Channel4_BASE = (AHBPERIPH_BASE + 0x0444);
constexpr intptr_t DMA2_Channel5_BASE = (AHBPERIPH_BASE + 0x0458);
constexpr intptr_t RCC_BASE           = (AHBPERIPH_BASE + 0x1000);
constexpr intptr_t CRC_BASE           = (AHBPERIPH_BASE + 0x3000);

/*!< Flash registers base address */
constexpr intptr_t FLASH_R_BASE = (AHBPERIPH_BASE + 0x2000);
/*!< Flash Option Bytes base address */
constexpr intptr_t OB_BASE = ((uint32_t)0x1FFFF800);

constexpr intptr_t ETH_BASE     = (AHBPERIPH_BASE + 0x8000);
constexpr intptr_t ETH_MAC_BASE = (ETH_BASE);
constexpr intptr_t ETH_MMC_BASE = (ETH_BASE + 0x0100);
constexpr intptr_t ETH_PTP_BASE = (ETH_BASE + 0x0700);
constexpr intptr_t ETH_DMA_BASE = (ETH_BASE + 0x1000);

/*!< FSMC Bank1 registers base address */
constexpr intptr_t FSMC_Bank1_R_BASE = (FSMC_R_BASE + 0x0000);
/*!< FSMC Bank1E registers base address */
constexpr intptr_t FSMC_Bank1E_R_BASE = (FSMC_R_BASE + 0x0104);
/*!< FSMC Bank2 registers base address */
constexpr intptr_t FSMC_Bank2_R_BASE = (FSMC_R_BASE + 0x0060);
/*!< FSMC Bank3 registers base address */
constexpr intptr_t FSMC_Bank3_R_BASE = (FSMC_R_BASE + 0x0080);
/*!< FSMC Bank4 registers base address */
constexpr intptr_t FSMC_Bank4_R_BASE = (FSMC_R_BASE + 0x00A0);
/*!< Debug MCU registers base address */
constexpr intptr_t DBGMCU_BASE = 0xE0042000;

/**
 * @}
 */

/** @addtogroup Peripheral_declaration
 * @{
 */

inline auto * const TIM2   = reinterpret_cast<TIM_TypeDef *>(TIM2_BASE);
inline auto * const TIM3   = reinterpret_cast<TIM_TypeDef *>(TIM3_BASE);
inline auto * const TIM4   = reinterpret_cast<TIM_TypeDef *>(TIM4_BASE);
inline auto * const TIM5   = reinterpret_cast<TIM_TypeDef *>(TIM5_BASE);
inline auto * const TIM6   = reinterpret_cast<TIM_TypeDef *>(TIM6_BASE);
inline auto * const TIM7   = reinterpret_cast<TIM_TypeDef *>(TIM7_BASE);
inline auto * const TIM12  = reinterpret_cast<TIM_TypeDef *>(TIM12_BASE);
inline auto * const TIM13  = reinterpret_cast<TIM_TypeDef *>(TIM13_BASE);
inline auto * const TIM14  = reinterpret_cast<TIM_TypeDef *>(TIM14_BASE);
inline auto * const RTC    = reinterpret_cast<RTC_TypeDef *>(RTC_BASE);
inline auto * const WWDG   = reinterpret_cast<WWDG_TypeDef *>(WWDG_BASE);
inline auto * const IWDG   = reinterpret_cast<IWDG_TypeDef *>(IWDG_BASE);
inline auto * const SPI2   = reinterpret_cast<SPI_TypeDef *>(SPI2_BASE);
inline auto * const SPI3   = reinterpret_cast<SPI_TypeDef *>(SPI3_BASE);
inline auto * const USART2 = reinterpret_cast<USART_TypeDef *>(USART2_BASE);
inline auto * const USART3 = reinterpret_cast<USART_TypeDef *>(USART3_BASE);
inline auto * const UART4  = reinterpret_cast<USART_TypeDef *>(UART4_BASE);
inline auto * const UART5  = reinterpret_cast<USART_TypeDef *>(UART5_BASE);
inline auto * const I2C1   = reinterpret_cast<I2C_TypeDef *>(I2C1_BASE);
inline auto * const I2C2   = reinterpret_cast<I2C_TypeDef *>(I2C2_BASE);
inline auto * const CAN1   = reinterpret_cast<CAN_TypeDef *>(CAN1_BASE);
inline auto * const CAN2   = reinterpret_cast<CAN_TypeDef *>(CAN2_BASE);
inline auto * const BKP    = reinterpret_cast<BKP_TypeDef *>(BKP_BASE);
inline auto * const PWR    = reinterpret_cast<PWR_TypeDef *>(PWR_BASE);
inline auto * const DAC    = reinterpret_cast<DAC_TypeDef *>(DAC_BASE);
inline auto * const CEC    = reinterpret_cast<CEC_TypeDef *>(CEC_BASE);
inline auto * const AFIO   = reinterpret_cast<AFIO_TypeDef *>(AFIO_BASE);
inline auto * const EXTI   = reinterpret_cast<EXTI_TypeDef *>(EXTI_BASE);
inline auto * const GPIOA  = reinterpret_cast<GPIO_TypeDef *>(GPIOA_BASE);
inline auto * const GPIOB  = reinterpret_cast<GPIO_TypeDef *>(GPIOB_BASE);
inline auto * const GPIOC  = reinterpret_cast<GPIO_TypeDef *>(GPIOC_BASE);
inline auto * const GPIOD  = reinterpret_cast<GPIO_TypeDef *>(GPIOD_BASE);
inline auto * const GPIOE  = reinterpret_cast<GPIO_TypeDef *>(GPIOE_BASE);
inline auto * const GPIOF  = reinterpret_cast<GPIO_TypeDef *>(GPIOF_BASE);
inline auto * const GPIOG  = reinterpret_cast<GPIO_TypeDef *>(GPIOG_BASE);
inline auto * const ADC1   = reinterpret_cast<ADC_TypeDef *>(ADC1_BASE);
inline auto * const ADC2   = reinterpret_cast<ADC_TypeDef *>(ADC2_BASE);
inline auto * const TIM1   = reinterpret_cast<TIM_TypeDef *>(TIM1_BASE);
inline auto * const SPI1   = reinterpret_cast<SPI_TypeDef *>(SPI1_BASE);
inline auto * const TIM8   = reinterpret_cast<TIM_TypeDef *>(TIM8_BASE);
inline auto * const USART1 = reinterpret_cast<USART_TypeDef *>(USART1_BASE);
inline auto * const ADC3   = reinterpret_cast<ADC_TypeDef *>(ADC3_BASE);
inline auto * const TIM15  = reinterpret_cast<TIM_TypeDef *>(TIM15_BASE);
inline auto * const TIM16  = reinterpret_cast<TIM_TypeDef *>(TIM16_BASE);
inline auto * const TIM17  = reinterpret_cast<TIM_TypeDef *>(TIM17_BASE);
inline auto * const TIM9   = reinterpret_cast<TIM_TypeDef *>(TIM9_BASE);
inline auto * const TIM10  = reinterpret_cast<TIM_TypeDef *>(TIM10_BASE);
inline auto * const TIM11  = reinterpret_cast<TIM_TypeDef *>(TIM11_BASE);
inline auto * const SDIO   = reinterpret_cast<SDIO_TypeDef *>(SDIO_BASE);
inline auto * const DMA1   = reinterpret_cast<DMA_TypeDef *>(DMA1_BASE);
inline auto * const DMA2   = reinterpret_cast<DMA_TypeDef *>(DMA2_BASE);
inline auto * const DMA1_Channel1 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA1_Channel1_BASE);
inline auto * const DMA1_Channel2 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA1_Channel2_BASE);
inline auto * const DMA1_Channel3 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA1_Channel3_BASE);
inline auto * const DMA1_Channel4 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA1_Channel4_BASE);
inline auto * const DMA1_Channel5 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA1_Channel5_BASE);
inline auto * const DMA1_Channel6 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA1_Channel6_BASE);
inline auto * const DMA1_Channel7 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA1_Channel7_BASE);
inline auto * const DMA2_Channel1 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA2_Channel1_BASE);
inline auto * const DMA2_Channel2 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA2_Channel2_BASE);
inline auto * const DMA2_Channel3 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA2_Channel3_BASE);
inline auto * const DMA2_Channel4 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA2_Channel4_BASE);
inline auto * const DMA2_Channel5 =
    reinterpret_cast<DMA_Channel_TypeDef *>(DMA2_Channel5_BASE);
inline auto * const RCC   = reinterpret_cast<RCC_TypeDef *>(RCC_BASE);
inline auto * const CRC   = reinterpret_cast<CRC_TypeDef *>(CRC_BASE);
inline auto * const FLASH = reinterpret_cast<FLASH_TypeDef *>(FLASH_R_BASE);
inline auto * const OB    = reinterpret_cast<OB_TypeDef *>(OB_BASE);
inline auto * const ETH   = reinterpret_cast<ETH_TypeDef *>(ETH_BASE);
inline auto * const FSMC_Bank1 =
    reinterpret_cast<FSMC_Bank1_TypeDef *>(FSMC_Bank1_R_BASE);
inline auto * const FSMC_Bank1E =
    reinterpret_cast<FSMC_Bank1E_TypeDef *>(FSMC_Bank1E_R_BASE);
inline auto * const FSMC_Bank2 =
    reinterpret_cast<FSMC_Bank2_TypeDef *>(FSMC_Bank2_R_BASE);
inline auto * const FSMC_Bank3 =
    reinterpret_cast<FSMC_Bank3_TypeDef *>(FSMC_Bank3_R_BASE);
inline auto * const FSMC_Bank4 =
    reinterpret_cast<FSMC_Bank4_TypeDef *>(FSMC_Bank4_R_BASE);
inline auto * const DBGMCU = reinterpret_cast<DBGMCU_TypeDef *>(DBGMCU_BASE);

// SJSU-Dev2: Putting contents of this include in sjsu::stm32f10x
}  // namespace sjsu::stm32f10x

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
