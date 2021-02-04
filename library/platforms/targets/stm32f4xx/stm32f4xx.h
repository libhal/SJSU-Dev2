#pragma once
/**
 ******************************************************************************
 * @file    stm32f4xx.h
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    30-September-2011
 * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer Header File.
 *          This file contains all the peripheral register's definitions, bits
 *          definitions and memory mapping for STM32F4xx devices.
 *
 *          The file is the unique include file that the application programmer
 *          is using in the C source code, usually in main.c. This file
 *contains:
 *           - Configuration section that allows to select:
 *              - The device used in the target application
 *              - To use or not the peripheral�s drivers in application
 *code(i.e. code will be based on direct access to peripheral�s registers rather
 *than drivers API), this option is controlled by
 *                "constexpr intptr_t USE_STDPERIPH_DRIVER = ( *              -
 *;To change few application-specific parameters such as the HSE crystal
 *frequency
 *           - Data structures and the address mapping for all peripherals
 *           - Peripheral's registers declarations and bits definition
 *           - Macros to access peripheral�s registers hardware
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

/** @addtogroup CMSIS
 * @{
 */

/** @addtogroup stm32f4xx
 * @{
 */

#include "platforms/processors/arm_cortex/m4/core_cm4.h"
#include <cstdint>

// SJSU-Dev2: Putting contents of this include in sjsu::stm32f4xx
namespace sjsu::stm32f4xx
{
/**
 * @brief Configuration of the Cortex-M4 Processor and Core Peripherals
 */
#define __MPU_PRESENT 1 /*!< STM32F4XX provides an MPU                     */
#undef __MPU_PRESENT
#undef __NVIC_PRIO_BITS
#define __NVIC_PRIO_BITS                               \
  4 /*!< STM32F4XX uses 4 Bits for the Priority Levels \
     */

#if !defined(__FPU_PRESENT)
#define __FPU_PRESENT 1 /*!< FPU present                                   */
#endif                  /* __FPU_PRESENT */

/**
 * @brief STM32F4XX Interrupt Number Definition, according to the selected
 * device in @ref Library_configuration_section
 */
enum IRQn
{
  /******  Cortex-M4 Processor Exceptions Numbers
   ****************************************************************/
  NonMaskableInt_IRQn   = -14, /*!< 2 Non Maskable Interrupt */
  MemoryManagement_IRQn = -12, /*!< 4 Cortex-M4 Memory Management Interrupt */
  BusFault_IRQn         = -11, /*!< 5 Cortex-M4 Bus Fault Interrupt */
  UsageFault_IRQn       = -10, /*!< 6 Cortex-M4 Usage Fault Interrupt */
  SVCall_IRQn           = -5,  /*!< 11 Cortex-M4 SV Call Interrupt  */
  DebugMonitor_IRQn     = -4,  /*!< 12 Cortex-M4 Debug Monitor Interrupt  */
  PendSV_IRQn           = -2,  /*!< 14 Cortex-M4 Pend SV Interrupt  */
  SysTick_IRQn          = -1,  /*!< 15 Cortex-M4 System Tick Interrupt  */
  /******  STM32 specific Interrupt Numbers
   **********************************************************************/
  WWDG_IRQn = 0, /*!< Window WatchDog Interrupt */
  PVD_IRQn  = 1, /*!< PVD through EXTI Line detection Interrupt */
  TAMP_STAMP_IRQn =
      2, /*!< Tamper and TimeStamp interrupts through the EXTI line */
  RTC_WKUP_IRQn     = 3,  /*!< RTC Wakeup interrupt through the EXTI line  */
  FLASH_IRQn        = 4,  /*!< FLASH global Interrupt  */
  RCC_IRQn          = 5,  /*!< RCC global Interrupt  */
  EXTI0_IRQn        = 6,  /*!< EXTI Line0 Interrupt  */
  EXTI1_IRQn        = 7,  /*!< EXTI Line1 Interrupt  */
  EXTI2_IRQn        = 8,  /*!< EXTI Line2 Interrupt  */
  EXTI3_IRQn        = 9,  /*!< EXTI Line3 Interrupt  */
  EXTI4_IRQn        = 10, /*!< EXTI Line4 Interrupt */
  DMA1_Stream0_IRQn = 11, /*!< DMA1 Stream 0 global Interrupt */
  DMA1_Stream1_IRQn = 12, /*!< DMA1 Stream 1 global Interrupt */
  DMA1_Stream2_IRQn = 13, /*!< DMA1 Stream 2 global Interrupt */
  DMA1_Stream3_IRQn = 14, /*!< DMA1 Stream 3 global Interrupt */
  DMA1_Stream4_IRQn = 15, /*!< DMA1 Stream 4 global Interrupt */
  DMA1_Stream5_IRQn = 16, /*!< DMA1 Stream 5 global Interrupt */
  DMA1_Stream6_IRQn = 17, /*!< DMA1 Stream 6 global Interrupt */
  ADC_IRQn          = 18, /*!< ADC1, ADC2 and ADC3 global Interrupts */
  CAN1_TX_IRQn      = 19, /*!< CAN1 TX Interrupt */
  CAN1_RX0_IRQn     = 20, /*!< CAN1 RX0 Interrupt */
  CAN1_RX1_IRQn     = 21, /*!< CAN1 RX1 Interrupt */
  CAN1_SCE_IRQn     = 22, /*!< CAN1 SCE Interrupt */
  EXTI9_5_IRQn      = 23, /*!< External Line[9:5] Interrupts */
  TIM1_BRK_TIM9_IRQn =
      24, /*!< TIM1 Break interrupt and TIM9 global interrupt */
  TIM1_UP_TIM10_IRQn =
      25, /*!< TIM1 Update Interrupt and TIM10 global interrupt */
  TIM1_TRG_COM_TIM11_IRQn = 26, /*!< TIM1 Trigger and Commutation Interrupt
                                   and TIM11 global interrupt */
  TIM1_CC_IRQn     = 27,        /*!< TIM1 Capture Compare Interrupt          */
  TIM2_IRQn        = 28,        /*!< TIM2 global Interrupt          */
  TIM3_IRQn        = 29,        /*!< TIM3 global Interrupt          */
  TIM4_IRQn        = 30,        /*!< TIM4 global Interrupt          */
  I2C1_EV_IRQn     = 31,        /*!< I2C1 Event Interrupt          */
  I2C1_ER_IRQn     = 32,        /*!< I2C1 Error Interrupt          */
  I2C2_EV_IRQn     = 33,        /*!< I2C2 Event Interrupt          */
  I2C2_ER_IRQn     = 34,        /*!< I2C2 Error Interrupt          */
  SPI1_IRQn        = 35,        /*!< SPI1 global Interrupt          */
  SPI2_IRQn        = 36,        /*!< SPI2 global Interrupt          */
  USART1_IRQn      = 37,        /*!< USART1 global Interrupt          */
  USART2_IRQn      = 38,        /*!< USART2 global Interrupt          */
  USART3_IRQn      = 39,        /*!< USART3 global Interrupt          */
  EXTI15_10_IRQn   = 40,        /*!< External Line[15:10] Interrupts          */
  RTC_Alarm_IRQn   = 41, /*!< RTC Alarm (A and B) through EXTI Line Interrupt */
  OTG_FS_WKUP_IRQn = 42, /*!< USB OTG FS Wakeup through EXTI line interrupt */
  TIM8_BRK_TIM12_IRQn =
      43, /*!< TIM8 Break Interrupt and TIM12 global interrupt */
  TIM8_UP_TIM13_IRQn =
      44, /*!< TIM8 Update Interrupt and TIM13 global interrupt */
  TIM8_TRG_COM_TIM14_IRQn = 45, /*!< TIM8 Trigger and Commutation Interrupt
                                   and TIM14 global interrupt */
  TIM8_CC_IRQn      = 46,       /*!< TIM8 Capture Compare Interrupt       */
  DMA1_Stream7_IRQn = 47,       /*!< DMA1 Stream7 Interrupt       */
  FSMC_IRQn         = 48,       /*!< FSMC global Interrupt       */
  SDIO_IRQn         = 49,       /*!< SDIO global Interrupt       */
  TIM5_IRQn         = 50,       /*!< TIM5 global Interrupt       */
  SPI3_IRQn         = 51,       /*!< SPI3 global Interrupt       */
  UART4_IRQn        = 52,       /*!< UART4 global Interrupt       */
  UART5_IRQn        = 53,       /*!< UART5 global Interrupt       */
  TIM6_DAC_IRQn = 54, /*!< TIM6 global and DAC1&2 underrun error  interrupts */
  TIM7_IRQn     = 55, /*!< TIM7 global interrupt */
  DMA2_Stream0_IRQn = 56, /*!< DMA2 Stream 0 global Interrupt */
  DMA2_Stream1_IRQn = 57, /*!< DMA2 Stream 1 global Interrupt */
  DMA2_Stream2_IRQn = 58, /*!< DMA2 Stream 2 global Interrupt */
  DMA2_Stream3_IRQn = 59, /*!< DMA2 Stream 3 global Interrupt */
  DMA2_Stream4_IRQn = 60, /*!< DMA2 Stream 4 global Interrupt */
  ETH_IRQn          = 61, /*!< Ethernet global Interrupt */
  ETH_WKUP_IRQn = 62, /*!< Ethernet Wakeup through EXTI line Interrupt     */
  CAN2_TX_IRQn  = 63, /*!< CAN2 TX Interrupt     */
  CAN2_RX0_IRQn = 64, /*!< CAN2 RX0 Interrupt     */
  CAN2_RX1_IRQn = 65, /*!< CAN2 RX1 Interrupt     */
  CAN2_SCE_IRQn = 66, /*!< CAN2 SCE Interrupt     */
  OTG_FS_IRQn   = 67, /*!< USB OTG FS global Interrupt     */
  DMA2_Stream5_IRQn   = 68, /*!< DMA2 Stream 5 global interrupt */
  DMA2_Stream6_IRQn   = 69, /*!< DMA2 Stream 6 global interrupt */
  DMA2_Stream7_IRQn   = 70, /*!< DMA2 Stream 7 global interrupt */
  USART6_IRQn         = 71, /*!< USART6 global interrupt */
  I2C3_EV_IRQn        = 72, /*!< I2C3 event interrupt */
  I2C3_ER_IRQn        = 73, /*!< I2C3 error interrupt */
  OTG_HS_EP1_OUT_IRQn = 74, /*!< USB OTG HS End Point 1 Out global interrupt */
  OTG_HS_EP1_IN_IRQn  = 75, /*!< USB OTG HS End Point 1 In global interrupt  */
  OTG_HS_WKUP_IRQn    = 76, /*!< USB OTG HS Wakeup through EXTI interrupt */
  OTG_HS_IRQn         = 77, /*!< USB OTG HS global interrupt */
  DCMI_IRQn           = 78, /*!< DCMI global interrupt */
  CRYP_IRQn           = 79, /*!< CRYP crypto global interrupt */
  HASH_RNG_IRQn       = 80, /*!< Hash and Rng global interrupt */
  FPU_IRQn            = 81  /*!< FPU global interrupt  */
};

/**
 * @}
 */
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
  _IO uint32_t SR;    /*!< ADC status register,                         Address
                          offset: 0x00 */
  _IO uint32_t CR1;   /*!< ADC control register 1,                      Address
                          offset: 0x04 */
  _IO uint32_t CR2;   /*!< ADC control register 2,                      Address
                          offset: 0x08 */
  _IO uint32_t SMPR1; /*!< ADC sample time register 1, Address offset: 0x0C
                       */
  _IO uint32_t SMPR2; /*!< ADC sample time register 2, Address offset: 0x10
                       */
  _IO uint32_t JOFR1; /*!< ADC injected channel data offset register 1,
                          Address offset: 0x14 */
  _IO uint32_t JOFR2; /*!< ADC injected channel data offset register 2,
                          Address offset: 0x18 */
  _IO uint32_t JOFR3; /*!< ADC injected channel data offset register 3,
                          Address offset: 0x1C */
  _IO uint32_t JOFR4; /*!< ADC injected channel data offset register 4,
                          Address offset: 0x20 */
  _IO uint32_t HTR;   /*!< ADC watchdog higher threshold register,      Address
                          offset: 0x24 */
  _IO uint32_t LTR;   /*!< ADC watchdog lower threshold register,       Address
                          offset: 0x28 */
  _IO uint32_t SQR1;  /*!< ADC regular sequence register 1, Address offset:
                          0x2C */
  _IO uint32_t SQR2;  /*!< ADC regular sequence register 2, Address offset:
                          0x30 */
  _IO uint32_t SQR3;  /*!< ADC regular sequence register 3, Address offset:
                          0x34 */
  _IO uint32_t JSQR;  /*!< ADC injected sequence register, Address offset:
                          0x38*/
  _IO uint32_t JDR1;  /*!< ADC injected data register 1, Address offset:
                          0x3C */
  _IO uint32_t JDR2;  /*!< ADC injected data register 2, Address offset:
                          0x40 */
  _IO uint32_t JDR3;  /*!< ADC injected data register 3, Address offset:
                          0x44 */
  _IO uint32_t JDR4;  /*!< ADC injected data register 4, Address offset:
                          0x48 */
  _IO uint32_t DR;    /*!< ADC regular data register,                   Address
                          offset: 0x4C */
} ADC_TypeDef;

typedef struct
{
  _IO uint32_t CSR; /*!< ADC Common status register,                  Address
                        offset: ADC1 base address + 0x300 */
  _IO uint32_t CCR; /*!< ADC common control register,                 Address
                        offset: ADC1 base address + 0x304 */
  _IO uint32_t CDR; /*!< ADC common regular data register for dual
                          AND triple modes,                            Address
                        offset: ADC1 base address + 0x308 */
} ADC_Common_TypeDef;

/**
 * @brief Controller Area Network TxMailBox
 */

typedef struct
{
  _IO uint32_t TIR; /*!< CAN TX mailbox identifier register */
  _IO uint32_t
      TDTR; /*!< CAN mailbox data length control and time stamp register */
  _IO uint32_t TDLR; /*!< CAN mailbox data low register */
  _IO uint32_t TDHR; /*!< CAN mailbox data high register */
} CAN_TxMailBox_TypeDef;

/**
 * @brief Controller Area Network FIFOMailBox
 */

typedef struct
{
  _IO uint32_t RIR;  /*!< CAN receive FIFO mailbox identifier register */
  _IO uint32_t RDTR; /*!< CAN receive FIFO mailbox data length control and
                         time stamp register */
  _IO uint32_t RDLR; /*!< CAN receive FIFO mailbox data low register */
  _IO uint32_t RDHR; /*!< CAN receive FIFO mailbox data high register */
} CAN_FIFOMailBox_TypeDef;

/**
 * @brief Controller Area Network FilterRegister
 */

typedef struct
{
  _IO uint32_t FR1; /*!< CAN Filter bank register 1 */
  _IO uint32_t FR2; /*!< CAN Filter bank register 1 */
} CAN_FilterRegister_TypeDef;

/**
 * @brief Controller Area Network
 */

typedef struct
{
  _IO uint32_t
      MCR; /*!< CAN master control register,         Address offset: 0x00 */
  _IO uint32_t
      MSR; /*!< CAN master status register,          Address offset: 0x04 */
  _IO uint32_t
      TSR; /*!< CAN transmit status register,        Address offset: 0x08 */
  _IO uint32_t RF0R; /*!< CAN receive FIFO 0 register,         Address
                         offset: 0x0C          */
  _IO uint32_t RF1R; /*!< CAN receive FIFO 1 register,         Address
                         offset: 0x10          */
  _IO uint32_t
      IER; /*!< CAN interrupt enable register,       Address offset: 0x14 */
  _IO uint32_t
      ESR; /*!< CAN error status register,           Address offset: 0x18 */
  _IO uint32_t
      BTR; /*!< CAN bit timing register,             Address offset: 0x1C */
  uint32_t RESERVED0[88]; /*!< Reserved, 0x020 - 0x17F              */
  CAN_TxMailBox_TypeDef sTxMailBox[3];     /*!< CAN Tx MailBox, Address offset:
                                              0x180 - 0x1AC */
  CAN_FIFOMailBox_TypeDef sFIFOMailBox[2]; /*!< CAN FIFO MailBox, Address
                                              offset: 0x1B0 - 0x1CC */
  uint32_t RESERVED1[12]; /*!< Reserved, 0x1D0 - 0x1FF                  */
  _IO uint32_t FMR;   /*!< CAN filter master register,          Address offset:
                          0x200         */
  _IO uint32_t FM1R;  /*!< CAN filter mode register,            Address
                          offset: 0x204         */
  uint32_t RESERVED2; /*!< Reserved, 0x208    */
  _IO uint32_t FS1R;  /*!< CAN filter scale register,           Address
                          offset: 0x20C         */
  uint32_t RESERVED3; /*!< Reserved, 0x210    */
  _IO uint32_t FFA1R; /*!< CAN filter FIFO assignment register, Address
                          offset: 0x214         */
  uint32_t RESERVED4; /*!< Reserved, 0x218    */
  _IO uint32_t FA1R;  /*!< CAN filter activation register,      Address
                          offset: 0x21C         */
  uint32_t RESERVED5[8]; /*!< Reserved, 0x220-0x23F */
  CAN_FilterRegister_TypeDef
      sFilterRegister[28]; /*!< CAN Filter Register,                 Address
                              offset: 0x240-0x31C   */
} CAN_TypeDef;

/**
 * @brief CRC calculation unit
 */

typedef struct
{
  _IO uint32_t DR;   /*!< CRC Data register,             Address offset: 0x00 */
  _IO uint8_t IDR;   /*!< CRC Independent data register, Address offset: 0x04 */
  uint8_t RESERVED0; /*!< Reserved, 0x05  */
  uint16_t RESERVED1; /*!< Reserved, 0x06 */
  _IO uint32_t CR; /*!< CRC Control register,          Address offset: 0x08 */
} CRC_TypeDef;

/**
 * @brief Digital to Analog Converter
 */

typedef struct
{
  _IO uint32_t CR; /*!< DAC control register, Address offset: 0x00 */
  _IO uint32_t
      SWTRIGR; /*!< DAC software trigger register, Address offset: 0x04 */
  _IO uint32_t DHR12R1; /*!< DAC channel1 12-bit right-aligned data holding
                            register, Address offset: 0x08 */
  _IO uint32_t DHR12L1; /*!< DAC channel1 12-bit left aligned data holding
                            register,  Address offset: 0x0C */
  _IO uint32_t DHR8R1;  /*!< DAC channel1 8-bit right aligned data holding
                            register,  Address offset: 0x10 */
  _IO uint32_t DHR12R2; /*!< DAC channel2 12-bit right aligned data holding
                            register, Address offset: 0x14 */
  _IO uint32_t DHR12L2; /*!< DAC channel2 12-bit left aligned data holding
                            register,  Address offset: 0x18 */
  _IO uint32_t DHR8R2;  /*!< DAC channel2 8-bit right-aligned data holding
                            register,  Address offset: 0x1C */
  _IO uint32_t DHR12RD; /*!< Dual DAC 12-bit right-aligned data holding
                            register,     Address offset: 0x20 */
  _IO uint32_t DHR12LD; /*!< DUAL DAC 12-bit left aligned data holding
                            register,      Address offset: 0x24 */
  _IO uint32_t DHR8RD;  /*!< DUAL DAC 8-bit right aligned data holding
                            register,      Address offset: 0x28 */
  _IO uint32_t
      DOR1; /*!< DAC channel1 data output register, Address offset: 0x2C */
  _IO uint32_t
      DOR2; /*!< DAC channel2 data output register, Address offset: 0x30 */
  _IO uint32_t SR; /*!< DAC status register, Address offset: 0x34 */
} DAC_TypeDef;

/**
 * @brief Debug MCU
 */

typedef struct
{
  _IO uint32_t
      IDCODE; /*!< MCU device ID code,               Address offset: 0x00 */
  _IO uint32_t
      CR; /*!< Debug MCU configuration register, Address offset: 0x04 */
  _IO uint32_t
      APB1FZ; /*!< Debug MCU APB1 freeze register,   Address offset: 0x08 */
  _IO uint32_t
      APB2FZ; /*!< Debug MCU APB2 freeze register,   Address offset: 0x0C */
} DBGMCU_TypeDef;

/**
 * @brief DCMI
 */

typedef struct
{
  _IO uint32_t CR;      /*!< DCMI control register 1,   Address offset: 0x00 */
  _IO uint32_t SR;      /*!< DCMI status register,   Address offset: 0x04 */
  _IO uint32_t RISR;    /*!< DCMI raw interrupt status register, Address
                            offset: 0x08 */
  _IO uint32_t IER;     /*!< DCMI interrupt enable register,  Address offset:
                            0x0C */
  _IO uint32_t MISR;    /*!< DCMI masked interrupt status register, Address
                            offset: 0x10 */
  _IO uint32_t ICR;     /*!< DCMI interrupt clear register,  Address offset:
                            0x14 */
  _IO uint32_t ESCR;    /*!< DCMI embedded synchronization code register,
                            Address offset: 0x18 */
  _IO uint32_t ESUR;    /*!< DCMI embedded synchronization unmask register,
                            Address offset: 0x1C */
  _IO uint32_t CWSTRTR; /*!< DCMI crop window start,  Address offset: 0x20 */
  _IO uint32_t CWSIZER; /*!< DCMI crop window size, Address offset: 0x24 */
  _IO uint32_t DR;      /*!< DCMI data register,      Address offset: 0x28 */
} DCMI_TypeDef;

/**
 * @brief DMA Controller
 */

typedef struct
{
  _IO uint32_t CR;   /*!< DMA stream x configuration register      */
  _IO uint32_t NDTR; /*!< DMA stream x number of data register     */
  _IO uint32_t PAR;  /*!< DMA stream x peripheral address register */
  _IO uint32_t M0AR; /*!< DMA stream x memory 0 address register   */
  _IO uint32_t M1AR; /*!< DMA stream x memory 1 address register   */
  _IO uint32_t FCR;  /*!< DMA stream x FIFO control register       */
} DMA_Stream_TypeDef;

typedef struct
{
  _IO uint32_t LISR;  /*!< DMA low interrupt status register,      Address
                          offset: 0x00 */
  _IO uint32_t HISR;  /*!< DMA high interrupt status register,     Address
                          offset: 0x04 */
  _IO uint32_t LIFCR; /*!< DMA low interrupt flag clear register,  Address
                          offset: 0x08 */
  _IO uint32_t HIFCR; /*!< DMA high interrupt flag clear register, Address
                          offset: 0x0C */
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
  _IO uint32_t RESERVED8;
  _IO uint32_t PTPTSSR;
  uint32_t RESERVED9[565];
  _IO uint32_t DMABMR;
  _IO uint32_t DMATPDR;
  _IO uint32_t DMARPDR;
  _IO uint32_t DMARDLAR;
  _IO uint32_t DMATDLAR;
  _IO uint32_t DMASR;
  _IO uint32_t DMAOMR;
  _IO uint32_t DMAIER;
  _IO uint32_t DMAMFBOCR;
  _IO uint32_t DMARSWTR;
  uint32_t RESERVED10[8];
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
  _IO uint32_t IMR;   /*!< EXTI Interrupt mask register,            Address
                          offset: 0x00 */
  _IO uint32_t EMR;   /*!< EXTI Event mask register,                Address
                          offset: 0x04 */
  _IO uint32_t RTSR;  /*!< EXTI Rising trigger selection register,  Address
                          offset: 0x08 */
  _IO uint32_t FTSR;  /*!< EXTI Falling trigger selection register, Address
                          offset: 0x0C */
  _IO uint32_t SWIER; /*!< EXTI Software interrupt event register,  Address
                          offset: 0x10 */
  _IO uint32_t PR;    /*!< EXTI Pending register,                   Address
                          offset: 0x14 */
} EXTI_TypeDef;

/**
 * @brief FLASH Registers
 */

typedef struct
{
  _IO uint32_t ACR;  /*!< FLASH access control register, Address offset: 0x00 */
  _IO uint32_t KEYR; /*!< FLASH key register,            Address offset: 0x04 */
  _IO uint32_t
      OPTKEYR;     /*!< FLASH option key register,     Address offset: 0x08 */
  _IO uint32_t SR; /*!< FLASH status register,         Address offset: 0x0C */
  _IO uint32_t CR; /*!< FLASH control register,        Address offset: 0x10 */
  _IO uint32_t
      OPTCR; /*!< FLASH option control register, Address offset: 0x14 */
} FLASH_TypeDef;

/**
 * @brief Flexible Static Memory Controller
 */

typedef struct
{
  _IO uint32_t
      BTCR[8]; /*!< NOR/PSRAM chip-select control register(BCR) and
                  chip-select timing register(BTR), Address offset: 0x00-1C */
} FSMC_Bank1_TypeDef;

/**
 * @brief Flexible Static Memory Controller Bank1E
 */

typedef struct
{
  _IO uint32_t BWTR[7]; /*!< NOR/PSRAM write timing registers, Address
                            offset: 0x104-0x11C */
} FSMC_Bank1E_TypeDef;

/**
 * @brief Flexible Static Memory Controller Bank2
 */

typedef struct
{
  _IO uint32_t PCR2;  /*!< NAND Flash control register 2,  Address offset:
                          0x60 */
  _IO uint32_t SR2;   /*!< NAND Flash FIFO status and interrupt register 2,
                          Address offset: 0x64 */
  _IO uint32_t PMEM2; /*!< NAND Flash Common memory space timing register 2,
                          Address offset: 0x68 */
  _IO uint32_t PATT2; /*!< NAND Flash Attribute memory space timing register
                          2, Address offset: 0x6C */
  uint32_t RESERVED0; /*!< Reserved, 0x70  */
  _IO uint32_t ECCR2; /*!< NAND Flash ECC result registers 2, Address
                          offset: 0x74 */
} FSMC_Bank2_TypeDef;

/**
 * @brief Flexible Static Memory Controller Bank3
 */

typedef struct
{
  _IO uint32_t PCR3;  /*!< NAND Flash control register 3,  Address offset:
                          0x80 */
  _IO uint32_t SR3;   /*!< NAND Flash FIFO status and interrupt register 3,
                          Address offset: 0x84 */
  _IO uint32_t PMEM3; /*!< NAND Flash Common memory space timing register 3,
                          Address offset: 0x88 */
  _IO uint32_t PATT3; /*!< NAND Flash Attribute memory space timing register
                          3, Address offset: 0x8C */
  uint32_t RESERVED0; /*!< Reserved, 0x90  */
  _IO uint32_t ECCR3; /*!< NAND Flash ECC result registers 3, Address
                          offset: 0x94 */
} FSMC_Bank3_TypeDef;

/**
 * @brief Flexible Static Memory Controller Bank4
 */

typedef struct
{
  _IO uint32_t PCR4;  /*!< PC Card  control register 4,  Address offset: 0xA0
                       */
  _IO uint32_t SR4;   /*!< PC Card  FIFO status and interrupt register 4,
                          Address offset: 0xA4 */
  _IO uint32_t PMEM4; /*!< PC Card  Common memory space timing register 4,
                          Address offset: 0xA8 */
  _IO uint32_t PATT4; /*!< PC Card  Attribute memory space timing register 4,
                          Address offset: 0xAC */
  _IO uint32_t PIO4;  /*!< PC Card  I/O space timing register 4,  Address
                          offset: 0xB0 */
} FSMC_Bank4_TypeDef;

/**
 * @brief General Purpose I/O
 */

typedef struct
{
  _IO uint32_t MODER;   /*!< GPIO port mode register,               Address
                            offset: 0x00      */
  _IO uint32_t OTYPER;  /*!< GPIO port output type register,        Address
                            offset: 0x04      */
  _IO uint32_t OSPEEDR; /*!< GPIO port output speed register,       Address
                            offset: 0x08      */
  _IO uint32_t PUPDR;   /*!< GPIO port pull-up/pull-down register,  Address
                            offset: 0x0C      */
  _IO uint32_t IDR;     /*!< GPIO port input data register,         Address
                            offset: 0x10      */
  _IO uint32_t ODR;     /*!< GPIO port output data register,        Address
                            offset: 0x14      */
  _IO uint16_t BSRRL;   /*!< GPIO port bit set/reset low register,  Address
                            offset: 0x18      */
  _IO uint16_t BSRRH;   /*!< GPIO port bit set/reset high register, Address
                            offset: 0x1A      */
  _IO uint32_t LCKR;    /*!< GPIO port configuration lock register, Address
                            offset: 0x1C      */
  _IO uint32_t AFR[2];  /*!< GPIO alternate function registers,     Address
                            offset: 0x20-0x24 */
} GPIO_TypeDef;

/**
 * @brief System configuration controller
 */

typedef struct
{
  _IO uint32_t MEMRMP;    /*!< SYSCFG memory remap register,    Address offset:
                              0x00      */
  _IO uint32_t PMC;       /*!< SYSCFG peripheral mode configuration register,
                              Address offset: 0x04      */
  _IO uint32_t EXTICR[4]; /*!< SYSCFG external interrupt configuration
                              registers, Address offset: 0x08-0x14 */
  uint32_t RESERVED[2];   /*!< Reserved, 0x18-0x1C    */
  _IO uint32_t CMPCR;     /*!< SYSCFG Compensation cell control register,
                              Address offset: 0x20      */
} SYSCFG_TypeDef;

/**
 * @brief Inter-integrated Circuit Interface
 */

typedef struct
{
  _IO uint16_t CR1;   /*!< I2C Control register 1,     Address offset: 0x00 */
  uint16_t RESERVED0; /*!< Reserved, 0x02                                   */
  _IO uint16_t CR2;   /*!< I2C Control register 2,     Address offset: 0x04 */
  uint16_t RESERVED1; /*!< Reserved, 0x06                                   */
  _IO uint16_t OAR1;  /*!< I2C Own address register 1, Address offset: 0x08 */
  uint16_t RESERVED2; /*!< Reserved, 0x0A                                   */
  _IO uint16_t OAR2;  /*!< I2C Own address register 2, Address offset: 0x0C */
  uint16_t RESERVED3; /*!< Reserved, 0x0E                                   */
  _IO uint16_t DR;    /*!< I2C Data register,          Address offset: 0x10 */
  uint16_t RESERVED4; /*!< Reserved, 0x12                                   */
  _IO uint16_t SR1;   /*!< I2C Status register 1,      Address offset: 0x14 */
  uint16_t RESERVED5; /*!< Reserved, 0x16                                   */
  _IO uint16_t SR2;   /*!< I2C Status register 2,      Address offset: 0x18 */
  uint16_t RESERVED6; /*!< Reserved, 0x1A                                   */
  _IO uint16_t CCR;   /*!< I2C Clock control register, Address offset: 0x1C */
  uint16_t RESERVED7; /*!< Reserved, 0x1E                                   */
  _IO uint16_t TRISE; /*!< I2C TRISE register,         Address offset: 0x20 */
  uint16_t RESERVED8; /*!< Reserved, 0x22                                   */
} I2C_TypeDef;

/**
 * @brief Independent WATCHDOG
 */

typedef struct
{
  _IO uint32_t KR;  /*!< IWDG Key register,       Address offset: 0x00 */
  _IO uint32_t PR;  /*!< IWDG Prescaler register, Address offset: 0x04 */
  _IO uint32_t RLR; /*!< IWDG Reload register,    Address offset: 0x08 */
  _IO uint32_t SR;  /*!< IWDG Status register,    Address offset: 0x0C */
} IWDG_TypeDef;

/**
 * @brief Power Control
 */

typedef struct
{
  _IO uint32_t
      CR; /*!< PWR power control register,        Address offset: 0x00 */
  _IO uint32_t
      CSR; /*!< PWR power control/status register, Address offset: 0x04 */
} PWR_TypeDef;

/**
 * @brief Reset and Clock Control
 */

typedef struct
{
  _IO uint32_t CR; /*!< RCC clock control register, Address offset: 0x00 */
  _IO uint32_t
      PLLCFGR; /*!< RCC PLL configuration register, Address offset: 0x04 */
  _IO uint32_t
      CFGR; /*!< RCC clock configuration register, Address offset: 0x08 */
  _IO uint32_t CIR; /*!< RCC clock interrupt register, Address offset: 0x0C */
  _IO uint32_t AHB1RSTR;  /*!< RCC AHB1 peripheral reset register,  Address
                              offset: 0x10 */
  _IO uint32_t AHB2RSTR;  /*!< RCC AHB2 peripheral reset register,  Address
                              offset: 0x14 */
  _IO uint32_t AHB3RSTR;  /*!< RCC AHB3 peripheral reset register,  Address
                              offset: 0x18 */
  uint32_t RESERVED0;     /*!< Reserved, 0x1C      */
  _IO uint32_t APB1RSTR;  /*!< RCC APB1 peripheral reset register,  Address
                              offset: 0x20 */
  _IO uint32_t APB2RSTR;  /*!< RCC APB2 peripheral reset register,  Address
                              offset: 0x24 */
  uint32_t RESERVED1[2];  /*!< Reserved, 0x28-0x2C   */
  _IO uint32_t AHB1ENR;   /*!< RCC AHB1 peripheral clock register,   Address
                              offset: 0x30 */
  _IO uint32_t AHB2ENR;   /*!< RCC AHB2 peripheral clock register,   Address
                              offset: 0x34 */
  _IO uint32_t AHB3ENR;   /*!< RCC AHB3 peripheral clock register,   Address
                              offset: 0x38 */
  uint32_t RESERVED2;     /*!< Reserved, 0x3C      */
  _IO uint32_t APB1ENR;   /*!< RCC APB1 peripheral clock enable register,
                              Address offset: 0x40 */
  _IO uint32_t APB2ENR;   /*!< RCC APB2 peripheral clock enable register,
                              Address offset: 0x44 */
  uint32_t RESERVED3[2];  /*!< Reserved, 0x48-0x4C   */
  _IO uint32_t AHB1LPENR; /*!< RCC AHB1 peripheral clock enable in low power
                              mode register, Address offset: 0x50 */
  _IO uint32_t AHB2LPENR; /*!< RCC AHB2 peripheral clock enable in low power
                              mode register, Address offset: 0x54 */
  _IO uint32_t AHB3LPENR; /*!< RCC AHB3 peripheral clock enable in low power
                              mode register, Address offset: 0x58 */
  uint32_t RESERVED4;     /*!< Reserved, 0x5C      */
  _IO uint32_t APB1LPENR; /*!< RCC APB1 peripheral clock enable in low power
                              mode register, Address offset: 0x60 */
  _IO uint32_t APB2LPENR; /*!< RCC APB2 peripheral clock enable in low power
                              mode register, Address offset: 0x64 */
  uint32_t RESERVED5[2];  /*!< Reserved, 0x68-0x6C   */
  _IO uint32_t
      BDCR; /*!< RCC Backup domain control register, Address offset: 0x70 */
  _IO uint32_t
      CSR; /*!< RCC clock control & status register, Address offset: 0x74 */
  uint32_t RESERVED6[2];   /*!< Reserved, 0x78-0x7C */
  _IO uint32_t SSCGR;      /*!< RCC spread spectrum clock generation register,
                               Address offset: 0x80 */
  _IO uint32_t PLLI2SCFGR; /*!< RCC PLLI2S configuration register, Address
                               offset: 0x84 */
} RCC_TypeDef;

/**
 * @brief Real-Time Clock
 */

typedef struct
{
  _IO uint32_t TR;   /*!< RTC time register,   Address offset: 0x00 */
  _IO uint32_t DR;   /*!< RTC date register,   Address offset: 0x04 */
  _IO uint32_t CR;   /*!< RTC control register,   Address offset: 0x08 */
  _IO uint32_t ISR;  /*!< RTC initialization and status register,  Address
                         offset: 0x0C */
  _IO uint32_t PRER; /*!< RTC prescaler register, Address offset: 0x10 */
  _IO uint32_t WUTR; /*!< RTC wakeup timer register, Address offset: 0x14 */
  _IO uint32_t
      CALIBR; /*!< RTC calibration register,           Address offset: 0x18 */
  _IO uint32_t ALRMAR; /*!< RTC alarm A register, Address offset: 0x1C */
  _IO uint32_t ALRMBR; /*!< RTC alarm B register, Address offset: 0x20 */
  _IO uint32_t WPR; /*!< RTC write protection register, Address offset: 0x24 */
  _IO uint32_t SSR; /*!< RTC sub second register, Address offset: 0x28 */
  _IO uint32_t SHIFTR; /*!< RTC shift control register, Address offset: 0x2C */
  _IO uint32_t TSTR;  /*!< RTC time stamp time register, Address offset: 0x30 */
  _IO uint32_t TSDR;  /*!< RTC time stamp date register, Address offset: 0x34 */
  _IO uint32_t TSSSR; /*!< RTC time-stamp sub second register, Address
                          offset: 0x38 */
  _IO uint32_t CALR;  /*!< RTC calibration register,  Address offset: 0x3C */
  _IO uint32_t TAFCR; /*!< RTC tamper and alternate function configuration
                          register, Address offset: 0x40 */
  _IO uint32_t ALRMASSR; /*!< RTC alarm A sub second register, Address
                             offset: 0x44 */
  _IO uint32_t ALRMBSSR; /*!< RTC alarm B sub second register, Address
                             offset: 0x48 */
  uint32_t RESERVED7;    /*!< Reserved, 0x4C     */
  _IO uint32_t BKP0R;    /*!< RTC backup register 1,    Address offset: 0x50 */
  _IO uint32_t BKP1R;    /*!< RTC backup register 1,    Address offset: 0x54 */
  _IO uint32_t BKP2R;    /*!< RTC backup register 2,    Address offset: 0x58 */
  _IO uint32_t BKP3R;    /*!< RTC backup register 3,    Address offset: 0x5C */
  _IO uint32_t BKP4R;    /*!< RTC backup register 4,    Address offset: 0x60 */
  _IO uint32_t BKP5R;    /*!< RTC backup register 5,    Address offset: 0x64 */
  _IO uint32_t BKP6R;    /*!< RTC backup register 6,    Address offset: 0x68 */
  _IO uint32_t BKP7R;    /*!< RTC backup register 7,    Address offset: 0x6C */
  _IO uint32_t BKP8R;    /*!< RTC backup register 8,    Address offset: 0x70 */
  _IO uint32_t BKP9R;    /*!< RTC backup register 9,    Address offset: 0x74 */
  _IO uint32_t BKP10R;   /*!< RTC backup register 10,   Address offset: 0x78 */
  _IO uint32_t BKP11R;   /*!< RTC backup register 11,   Address offset: 0x7C */
  _IO uint32_t BKP12R;   /*!< RTC backup register 12,   Address offset: 0x80 */
  _IO uint32_t BKP13R;   /*!< RTC backup register 13,   Address offset: 0x84 */
  _IO uint32_t BKP14R;   /*!< RTC backup register 14,   Address offset: 0x88 */
  _IO uint32_t BKP15R;   /*!< RTC backup register 15,   Address offset: 0x8C */
  _IO uint32_t BKP16R;   /*!< RTC backup register 16,   Address offset: 0x90 */
  _IO uint32_t BKP17R;   /*!< RTC backup register 17,   Address offset: 0x94 */
  _IO uint32_t BKP18R;   /*!< RTC backup register 18,   Address offset: 0x98 */
  _IO uint32_t BKP19R;   /*!< RTC backup register 19,   Address offset: 0x9C */
} RTC_TypeDef;

/**
 * @brief SD host Interface
 */

typedef struct
{
  _IO uint32_t
      POWER; /*!< SDIO power control register,    Address offset: 0x00 */
  _IO uint32_t
      CLKCR;        /*!< SDI clock control register,     Address offset: 0x04 */
  _IO uint32_t ARG; /*!< SDIO argument register,         Address offset: 0x08 */
  _IO uint32_t CMD; /*!< SDIO command register,          Address offset: 0x0C */
  _I uint32_t
      RESPCMD; /*!< SDIO command response register, Address offset: 0x10 */
  _I uint32_t
      RESP1; /*!< SDIO response 1 register,       Address offset: 0x14 */
  _I uint32_t
      RESP2; /*!< SDIO response 2 register,       Address offset: 0x18 */
  _I uint32_t
      RESP3; /*!< SDIO response 3 register,       Address offset: 0x1C */
  _I uint32_t
      RESP4; /*!< SDIO response 4 register,       Address offset: 0x20 */
  _IO uint32_t
      DTIMER; /*!< SDIO data timer register,       Address offset: 0x24 */
  _IO uint32_t
      DLEN; /*!< SDIO data length register,      Address offset: 0x28 */
  _IO uint32_t
      DCTRL; /*!< SDIO data control register,     Address offset: 0x2C */
  _I uint32_t
      DCOUNT;       /*!< SDIO data counter register,     Address offset: 0x30 */
  _I uint32_t STA;  /*!< SDIO status register,           Address offset: 0x34 */
  _IO uint32_t ICR; /*!< SDIO interrupt clear register,  Address offset: 0x38 */
  _IO uint32_t
      MASK; /*!< SDIO mask register,             Address offset: 0x3C */
  uint32_t RESERVED0[2]; /*!< Reserved, 0x40-0x44 */
  _I uint32_t
      FIFOCNT; /*!< SDIO FIFO counter register,     Address offset: 0x48 */
  uint32_t RESERVED1[13]; /*!< Reserved, 0x4C-0x7C */
  _IO uint32_t
      FIFO; /*!< SDIO data FIFO register,        Address offset: 0x80 */
} SDIO_TypeDef;

/**
 * @brief Serial Peripheral Interface
 */

typedef struct
{
  _IO uint16_t CR1;    /*!< SPI control register 1 (not used in I2S mode),
                           Address offset: 0x00 */
  uint16_t RESERVED0;  /*!< Reserved, 0x02   */
  _IO uint16_t CR2;    /*!< SPI control register 2,    Address offset: 0x04 */
  uint16_t RESERVED1;  /*!< Reserved, 0x06   */
  _IO uint16_t SR;     /*!< SPI status register,     Address offset: 0x08 */
  uint16_t RESERVED2;  /*!< Reserved, 0x0A   */
  _IO uint16_t DR;     /*!< SPI data register,     Address offset: 0x0C */
  uint16_t RESERVED3;  /*!< Reserved, 0x0E   */
  _IO uint16_t CRCPR;  /*!< SPI CRC polynomial register (not used in I2S
                           mode), Address offset: 0x10 */
  uint16_t RESERVED4;  /*!< Reserved, 0x12   */
  _IO uint16_t RXCRCR; /*!< SPI RX CRC register (not used in I2S mode),
                           Address offset: 0x14 */
  uint16_t RESERVED5;  /*!< Reserved, 0x16   */
  _IO uint16_t TXCRCR; /*!< SPI TX CRC register (not used in I2S mode),
                           Address offset: 0x18 */
  uint16_t RESERVED6;  /*!< Reserved, 0x1A   */
  _IO uint16_t
      I2SCFGR; /*!< SPI_I2S configuration register, Address offset: 0x1C */
  uint16_t RESERVED7; /*!< Reserved, 0x1E  */
  _IO uint16_t I2SPR; /*!< SPI_I2S prescaler register, Address offset: 0x20
                       */
  uint16_t RESERVED8; /*!< Reserved, 0x22  */
} SPI_TypeDef;

/**
 * @brief TIM
 */

typedef struct
{
  _IO uint16_t
      CR1; /*!< TIM control register 1,              Address offset: 0x00 */
  uint16_t RESERVED0; /*!< Reserved, 0x02 */
  _IO uint16_t
      CR2; /*!< TIM control register 2,              Address offset: 0x04 */
  uint16_t RESERVED1; /*!< Reserved, 0x06 */
  _IO uint16_t
      SMCR; /*!< TIM slave mode control register,     Address offset: 0x08 */
  uint16_t RESERVED2; /*!< Reserved, 0x0A */
  _IO uint16_t
      DIER; /*!< TIM DMA/interrupt enable register,   Address offset: 0x0C */
  uint16_t RESERVED3; /*!< Reserved, 0x0E */
  _IO uint16_t
      SR; /*!< TIM status register,                 Address offset: 0x10 */
  uint16_t RESERVED4; /*!< Reserved, 0x12 */
  _IO uint16_t
      EGR; /*!< TIM event generation register,       Address offset: 0x14 */
  uint16_t RESERVED5; /*!< Reserved, 0x16 */
  _IO uint16_t
      CCMR1; /*!< TIM capture/compare mode register 1, Address offset: 0x18 */
  uint16_t RESERVED6; /*!< Reserved, 0x1A */
  _IO uint16_t
      CCMR2; /*!< TIM capture/compare mode register 2, Address offset: 0x1C */
  uint16_t RESERVED7; /*!< Reserved, 0x1E */
  _IO uint16_t
      CCER; /*!< TIM capture/compare enable register, Address offset: 0x20 */
  uint16_t RESERVED8; /*!< Reserved, 0x22 */
  _IO uint32_t
      CNT; /*!< TIM counter register,                Address offset: 0x24 */
  _IO uint16_t
      PSC; /*!< TIM prescaler,                       Address offset: 0x28 */
  uint16_t RESERVED9; /*!< Reserved, 0x2A */
  _IO uint32_t
      ARR; /*!< TIM auto-reload register,            Address offset: 0x2C */
  _IO uint16_t
      RCR; /*!< TIM repetition counter register,     Address offset: 0x30 */
  uint16_t RESERVED10; /*!< Reserved, 0x32 */
  _IO uint32_t
      CCR1; /*!< TIM capture/compare register 1,      Address offset: 0x34 */
  _IO uint32_t
      CCR2; /*!< TIM capture/compare register 2,      Address offset: 0x38 */
  _IO uint32_t
      CCR3; /*!< TIM capture/compare register 3,      Address offset: 0x3C */
  _IO uint32_t
      CCR4; /*!< TIM capture/compare register 4,      Address offset: 0x40 */
  _IO uint16_t
      BDTR; /*!< TIM break and dead-time register,    Address offset: 0x44 */
  uint16_t RESERVED11; /*!< Reserved, 0x46 */
  _IO uint16_t
      DCR; /*!< TIM DMA control register,            Address offset: 0x48 */
  uint16_t RESERVED12; /*!< Reserved, 0x4A */
  _IO uint16_t
      DMAR; /*!< TIM DMA address for full transfer,   Address offset: 0x4C */
  uint16_t RESERVED13; /*!< Reserved, 0x4E */
  _IO uint16_t
      OR; /*!< TIM option register,                 Address offset: 0x50 */
  uint16_t RESERVED14; /*!< Reserved, 0x52 */
} TIM_TypeDef;

/**
 * @brief Universal Synchronous Asynchronous Receiver Transmitter
 */

typedef struct
{
  _IO uint16_t SR;    /*!< USART Status register,                   Address
                          offset: 0x00 */
  uint16_t RESERVED0; /*!< Reserved, 0x02 */
  _IO uint16_t DR;    /*!< USART Data register,                     Address
                          offset: 0x04 */
  uint16_t RESERVED1; /*!< Reserved, 0x06 */
  _IO uint16_t BRR;   /*!< USART Baud rate register,                Address
                          offset: 0x08 */
  uint16_t RESERVED2; /*!< Reserved, 0x0A */
  _IO uint16_t CR1;   /*!< USART Control register 1,                Address
                          offset: 0x0C */
  uint16_t RESERVED3; /*!< Reserved, 0x0E */
  _IO uint16_t CR2;   /*!< USART Control register 2,                Address
                          offset: 0x10 */
  uint16_t RESERVED4; /*!< Reserved, 0x12 */
  _IO uint16_t CR3;   /*!< USART Control register 3,                Address
                          offset: 0x14 */
  uint16_t RESERVED5; /*!< Reserved, 0x16 */
  _IO uint16_t GTPR;  /*!< USART Guard time and prescaler register, Address
                          offset: 0x18 */
  uint16_t RESERVED6; /*!< Reserved, 0x1A */
} USART_TypeDef;

/**
 * @brief Window WATCHDOG
 */

typedef struct
{
  _IO uint32_t CR;  /*!< WWDG Control register,       Address offset: 0x00 */
  _IO uint32_t CFR; /*!< WWDG Configuration register, Address offset: 0x04 */
  _IO uint32_t SR;  /*!< WWDG Status register,        Address offset: 0x08 */
} WWDG_TypeDef;

/**
 * @brief Crypto Processor
 */

typedef struct
{
  _IO uint32_t CR;    /*!< CRYP control register, Address offset: 0x00 */
  _IO uint32_t SR;    /*!< CRYP status register, Address offset: 0x04 */
  _IO uint32_t DR;    /*!< CRYP data input register, Address offset: 0x08 */
  _IO uint32_t DOUT;  /*!< CRYP data output register,   Address offset: 0x0C */
  _IO uint32_t DMACR; /*!< CRYP DMA control register, Address offset: 0x10
                       */
  _IO uint32_t IMSCR; /*!< CRYP interrupt mask set/clear register, Address
                          offset: 0x14 */
  _IO uint32_t RISR;  /*!< CRYP raw interrupt status register,  Address
                          offset: 0x18 */
  _IO uint32_t MISR;  /*!< CRYP masked interrupt status register,  Address
                          offset: 0x1C */
  _IO uint32_t K0LR;  /*!< CRYP key left  register 0, Address offset: 0x20 */
  _IO uint32_t K0RR;  /*!< CRYP key right register 0, Address offset: 0x24 */
  _IO uint32_t K1LR;  /*!< CRYP key left  register 1, Address offset: 0x28 */
  _IO uint32_t K1RR;  /*!< CRYP key right register 1, Address offset: 0x2C */
  _IO uint32_t K2LR;  /*!< CRYP key left  register 2, Address offset: 0x30 */
  _IO uint32_t K2RR;  /*!< CRYP key right register 2, Address offset: 0x34 */
  _IO uint32_t K3LR;  /*!< CRYP key left  register 3, Address offset: 0x38 */
  _IO uint32_t K3RR;  /*!< CRYP key right register 3,   Address offset: 0x3C */
  _IO uint32_t IV0LR; /*!< CRYP initialization vector left-word  register 0,
                          Address offset: 0x40 */
  _IO uint32_t IV0RR; /*!< CRYP initialization vector right-word register 0,
                          Address offset: 0x44 */
  _IO uint32_t IV1LR; /*!< CRYP initialization vector left-word  register 1,
                          Address offset: 0x48 */
  _IO uint32_t IV1RR; /*!< CRYP initialization vector right-word register 1,
                          Address offset: 0x4C */
} CRYP_TypeDef;

/**
 * @brief HASH
 */

typedef struct
{
  _IO uint32_t
      CR; /*!< HASH control register,          Address offset: 0x00        */
  _IO uint32_t
      DIN; /*!< HASH data input register,       Address offset: 0x04        */
  _IO uint32_t
      STR; /*!< HASH start register,            Address offset: 0x08        */
  _IO uint32_t HR[5]; /*!< HASH digest registers,          Address offset:
                          0x0C-0x1C   */
  _IO uint32_t
      IMR; /*!< HASH interrupt enable register, Address offset: 0x20        */
  _IO uint32_t
      SR; /*!< HASH status register,           Address offset: 0x24        */
  uint32_t RESERVED[52]; /*!< Reserved, 0x28-0xF4 */
  _IO uint32_t CSR[51];  /*!< HASH context swap registers,    Address offset:
                             0x0F8-0x1C0 */
} HASH_TypeDef;

/**
 * @brief HASH
 */

typedef struct
{
  _IO uint32_t CR; /*!< RNG control register, Address offset: 0x00 */
  _IO uint32_t SR; /*!< RNG status register,  Address offset: 0x04 */
  _IO uint32_t DR; /*!< RNG data register,    Address offset: 0x08 */
} RNG_TypeDef;

/**
 * @}
 */

/** @addtogroup Peripheral_memory_map
 * @{
 */

/*!< FLASH(up to 1 MB) base address in the alias region */
constexpr intptr_t FLASH_BASE = 0x08000000;
/*!< CCM(core coupled memory) data RAM(64 KB) base address in the alias region
 */
constexpr intptr_t CCMDATARAM_BASE = 0x10000000;
/*!< SRAM1(112 KB) base address in the alias region   */
constexpr intptr_t SRAM1_BASE = 0x20000000;
/*!< SRAM2(16 KB) base address in the alias region   */
constexpr intptr_t SRAM2_BASE = 0x2001C000;
/*!< Backup SRAM(4 KB) base address in the alias region */
constexpr intptr_t BKPSRAM_BASE = 0x40024000;
/*!< Peripheral base address in the alias region */
constexpr intptr_t PERIPH_BASE = 0x40000000;
/*!< FSMC registers base address */
constexpr intptr_t FSMC_R_BASE = 0xA0000000;
/*!< CCM(core coupled memory) data RAM(64 KB) baseaddress in the bit-band
 * region */
constexpr intptr_t CCMDATARAM_BB_BASE = 0x12000000;
/*!< SRAM1(112 KB) base address in the bit-band region */
constexpr intptr_t SRAM1_BB_BASE = 0x22000000;
/*!< SRAM2(16 KB) base address in the bit-band region   */
constexpr intptr_t SRAM2_BB_BASE = 0x2201C000;
/*!< Peripheral base address in the bit-band region   */
constexpr intptr_t PERIPH_BB_BASE = 0x42000000;
/*!< Backup SRAM(4 KB) base address in the bit-band region */
constexpr intptr_t BKPSRAM_BB_BASE = 0x42024000;

/* Legacy defines */
constexpr intptr_t SRAM_BASE    = (SRAM1_BASE);
constexpr intptr_t SRAM_BB_BASE = (SRAM1_BB_BASE);

/*!< Peripheral memory map */
constexpr intptr_t APB1PERIPH_BASE = (PERIPH_BASE);
constexpr intptr_t APB2PERIPH_BASE = (PERIPH_BASE + 0x00010000);
constexpr intptr_t AHB1PERIPH_BASE = (PERIPH_BASE + 0x00020000);
constexpr intptr_t AHB2PERIPH_BASE = (PERIPH_BASE + 0x10000000);

/*!< APB1 peripherals */
constexpr intptr_t TIM2_BASE    = (APB1PERIPH_BASE + 0x0000);
constexpr intptr_t TIM3_BASE    = (APB1PERIPH_BASE + 0x0400);
constexpr intptr_t TIM4_BASE    = (APB1PERIPH_BASE + 0x0800);
constexpr intptr_t TIM5_BASE    = (APB1PERIPH_BASE + 0x0C00);
constexpr intptr_t TIM6_BASE    = (APB1PERIPH_BASE + 0x1000);
constexpr intptr_t TIM7_BASE    = (APB1PERIPH_BASE + 0x1400);
constexpr intptr_t TIM12_BASE   = (APB1PERIPH_BASE + 0x1800);
constexpr intptr_t TIM13_BASE   = (APB1PERIPH_BASE + 0x1C00);
constexpr intptr_t TIM14_BASE   = (APB1PERIPH_BASE + 0x2000);
constexpr intptr_t RTC_BASE     = (APB1PERIPH_BASE + 0x2800);
constexpr intptr_t WWDG_BASE    = (APB1PERIPH_BASE + 0x2C00);
constexpr intptr_t IWDG_BASE    = (APB1PERIPH_BASE + 0x3000);
constexpr intptr_t I2S2ext_BASE = (APB1PERIPH_BASE + 0x3400);
constexpr intptr_t SPI2_BASE    = (APB1PERIPH_BASE + 0x3800);
constexpr intptr_t SPI3_BASE    = (APB1PERIPH_BASE + 0x3C00);
constexpr intptr_t I2S3ext_BASE = (APB1PERIPH_BASE + 0x4000);
constexpr intptr_t USART2_BASE  = (APB1PERIPH_BASE + 0x4400);
constexpr intptr_t USART3_BASE  = (APB1PERIPH_BASE + 0x4800);
constexpr intptr_t UART4_BASE   = (APB1PERIPH_BASE + 0x4C00);
constexpr intptr_t UART5_BASE   = (APB1PERIPH_BASE + 0x5000);
constexpr intptr_t I2C1_BASE    = (APB1PERIPH_BASE + 0x5400);
constexpr intptr_t I2C2_BASE    = (APB1PERIPH_BASE + 0x5800);
constexpr intptr_t I2C3_BASE    = (APB1PERIPH_BASE + 0x5C00);
constexpr intptr_t CAN1_BASE    = (APB1PERIPH_BASE + 0x6400);
constexpr intptr_t CAN2_BASE    = (APB1PERIPH_BASE + 0x6800);
constexpr intptr_t PWR_BASE     = (APB1PERIPH_BASE + 0x7000);
constexpr intptr_t DAC_BASE     = (APB1PERIPH_BASE + 0x7400);

/*!< APB2 peripherals */
constexpr intptr_t TIM1_BASE   = (APB2PERIPH_BASE + 0x0000);
constexpr intptr_t TIM8_BASE   = (APB2PERIPH_BASE + 0x0400);
constexpr intptr_t USART1_BASE = (APB2PERIPH_BASE + 0x1000);
constexpr intptr_t USART6_BASE = (APB2PERIPH_BASE + 0x1400);
constexpr intptr_t ADC1_BASE   = (APB2PERIPH_BASE + 0x2000);
constexpr intptr_t ADC2_BASE   = (APB2PERIPH_BASE + 0x2100);
constexpr intptr_t ADC3_BASE   = (APB2PERIPH_BASE + 0x2200);
constexpr intptr_t ADC_BASE    = (APB2PERIPH_BASE + 0x2300);
constexpr intptr_t SDIO_BASE   = (APB2PERIPH_BASE + 0x2C00);
constexpr intptr_t SPI1_BASE   = (APB2PERIPH_BASE + 0x3000);
constexpr intptr_t SYSCFG_BASE = (APB2PERIPH_BASE + 0x3800);
constexpr intptr_t EXTI_BASE   = (APB2PERIPH_BASE + 0x3C00);
constexpr intptr_t TIM9_BASE   = (APB2PERIPH_BASE + 0x4000);
constexpr intptr_t TIM10_BASE  = (APB2PERIPH_BASE + 0x4400);
constexpr intptr_t TIM11_BASE  = (APB2PERIPH_BASE + 0x4800);

/*!< AHB1 peripherals */
constexpr intptr_t GPIOA_BASE        = (AHB1PERIPH_BASE + 0x0000);
constexpr intptr_t GPIOB_BASE        = (AHB1PERIPH_BASE + 0x0400);
constexpr intptr_t GPIOC_BASE        = (AHB1PERIPH_BASE + 0x0800);
constexpr intptr_t GPIOD_BASE        = (AHB1PERIPH_BASE + 0x0C00);
constexpr intptr_t GPIOE_BASE        = (AHB1PERIPH_BASE + 0x1000);
constexpr intptr_t GPIOF_BASE        = (AHB1PERIPH_BASE + 0x1400);
constexpr intptr_t GPIOG_BASE        = (AHB1PERIPH_BASE + 0x1800);
constexpr intptr_t GPIOH_BASE        = (AHB1PERIPH_BASE + 0x1C00);
constexpr intptr_t GPIOI_BASE        = (AHB1PERIPH_BASE + 0x2000);
constexpr intptr_t CRC_BASE          = (AHB1PERIPH_BASE + 0x3000);
constexpr intptr_t RCC_BASE          = (AHB1PERIPH_BASE + 0x3800);
constexpr intptr_t FLASH_R_BASE      = (AHB1PERIPH_BASE + 0x3C00);
constexpr intptr_t DMA1_BASE         = (AHB1PERIPH_BASE + 0x6000);
constexpr intptr_t DMA1_Stream0_BASE = (DMA1_BASE + 0x010);
constexpr intptr_t DMA1_Stream1_BASE = (DMA1_BASE + 0x028);
constexpr intptr_t DMA1_Stream2_BASE = (DMA1_BASE + 0x040);
constexpr intptr_t DMA1_Stream3_BASE = (DMA1_BASE + 0x058);
constexpr intptr_t DMA1_Stream4_BASE = (DMA1_BASE + 0x070);
constexpr intptr_t DMA1_Stream5_BASE = (DMA1_BASE + 0x088);
constexpr intptr_t DMA1_Stream6_BASE = (DMA1_BASE + 0x0A0);
constexpr intptr_t DMA1_Stream7_BASE = (DMA1_BASE + 0x0B8);
constexpr intptr_t DMA2_BASE         = (AHB1PERIPH_BASE + 0x6400);
constexpr intptr_t DMA2_Stream0_BASE = (DMA2_BASE + 0x010);
constexpr intptr_t DMA2_Stream1_BASE = (DMA2_BASE + 0x028);
constexpr intptr_t DMA2_Stream2_BASE = (DMA2_BASE + 0x040);
constexpr intptr_t DMA2_Stream3_BASE = (DMA2_BASE + 0x058);
constexpr intptr_t DMA2_Stream4_BASE = (DMA2_BASE + 0x070);
constexpr intptr_t DMA2_Stream5_BASE = (DMA2_BASE + 0x088);
constexpr intptr_t DMA2_Stream6_BASE = (DMA2_BASE + 0x0A0);
constexpr intptr_t DMA2_Stream7_BASE = (DMA2_BASE + 0x0B8);
constexpr intptr_t ETH_BASE          = (AHB1PERIPH_BASE + 0x8000);
constexpr intptr_t ETH_MAC_BASE      = (ETH_BASE);
constexpr intptr_t ETH_MMC_BASE      = (ETH_BASE + 0x0100);
constexpr intptr_t ETH_PTP_BASE      = (ETH_BASE + 0x0700);
constexpr intptr_t ETH_DMA_BASE      = (ETH_BASE + 0x1000);

/*!< AHB2 peripherals */
constexpr intptr_t DCMI_BASE = (AHB2PERIPH_BASE + 0x50000);
constexpr intptr_t CRYP_BASE = (AHB2PERIPH_BASE + 0x60000);
constexpr intptr_t HASH_BASE = (AHB2PERIPH_BASE + 0x60400);
constexpr intptr_t RNG_BASE  = (AHB2PERIPH_BASE + 0x60800);

/*!< FSMC Bankx registers base address */
constexpr intptr_t FSMC_Bank1_R_BASE  = (FSMC_R_BASE + 0x0000);
constexpr intptr_t FSMC_Bank1E_R_BASE = (FSMC_R_BASE + 0x0104);
constexpr intptr_t FSMC_Bank2_R_BASE  = (FSMC_R_BASE + 0x0060);
constexpr intptr_t FSMC_Bank3_R_BASE  = (FSMC_R_BASE + 0x0080);
constexpr intptr_t FSMC_Bank4_R_BASE  = (FSMC_R_BASE + 0x00A0);

/* Debug MCU registers base address */
constexpr intptr_t DBGMCU_BASE = (0xE0042000);

/**
 * @}
 */

/** @addtogroup Peripheral_declaration
 * @{
 */
inline auto * const TIM2    = reinterpret_cast<TIM_TypeDef *>(TIM2_BASE);
inline auto * const TIM3    = reinterpret_cast<TIM_TypeDef *>(TIM3_BASE);
inline auto * const TIM4    = reinterpret_cast<TIM_TypeDef *>(TIM4_BASE);
inline auto * const TIM5    = reinterpret_cast<TIM_TypeDef *>(TIM5_BASE);
inline auto * const TIM6    = reinterpret_cast<TIM_TypeDef *>(TIM6_BASE);
inline auto * const TIM7    = reinterpret_cast<TIM_TypeDef *>(TIM7_BASE);
inline auto * const TIM12   = reinterpret_cast<TIM_TypeDef *>(TIM12_BASE);
inline auto * const TIM13   = reinterpret_cast<TIM_TypeDef *>(TIM13_BASE);
inline auto * const TIM14   = reinterpret_cast<TIM_TypeDef *>(TIM14_BASE);
inline auto * const RTC     = reinterpret_cast<RTC_TypeDef *>(RTC_BASE);
inline auto * const WWDG    = reinterpret_cast<WWDG_TypeDef *>(WWDG_BASE);
inline auto * const IWDG    = reinterpret_cast<IWDG_TypeDef *>(IWDG_BASE);
inline auto * const I2S2ext = reinterpret_cast<SPI_TypeDef *>(I2S2ext_BASE);
inline auto * const SPI2    = reinterpret_cast<SPI_TypeDef *>(SPI2_BASE);
inline auto * const SPI3    = reinterpret_cast<SPI_TypeDef *>(SPI3_BASE);
inline auto * const I2S3ext = reinterpret_cast<SPI_TypeDef *>(I2S3ext_BASE);
inline auto * const USART2  = reinterpret_cast<USART_TypeDef *>(USART2_BASE);
inline auto * const USART3  = reinterpret_cast<USART_TypeDef *>(USART3_BASE);
inline auto * const UART4   = reinterpret_cast<USART_TypeDef *>(UART4_BASE);
inline auto * const UART5   = reinterpret_cast<USART_TypeDef *>(UART5_BASE);
inline auto * const I2C1    = reinterpret_cast<I2C_TypeDef *>(I2C1_BASE);
inline auto * const I2C2    = reinterpret_cast<I2C_TypeDef *>(I2C2_BASE);
inline auto * const I2C3    = reinterpret_cast<I2C_TypeDef *>(I2C3_BASE);
inline auto * const CAN1    = reinterpret_cast<CAN_TypeDef *>(CAN1_BASE);
inline auto * const CAN2    = reinterpret_cast<CAN_TypeDef *>(CAN2_BASE);
inline auto * const PWR     = reinterpret_cast<PWR_TypeDef *>(PWR_BASE);
inline auto * const DAC     = reinterpret_cast<DAC_TypeDef *>(DAC_BASE);
inline auto * const TIM1    = reinterpret_cast<TIM_TypeDef *>(TIM1_BASE);
inline auto * const TIM8    = reinterpret_cast<TIM_TypeDef *>(TIM8_BASE);
inline auto * const USART1  = reinterpret_cast<USART_TypeDef *>(USART1_BASE);
inline auto * const USART6  = reinterpret_cast<USART_TypeDef *>(USART6_BASE);
inline auto * const ADC     = reinterpret_cast<ADC_Common_TypeDef *>(ADC_BASE);
inline auto * const ADC1    = reinterpret_cast<ADC_TypeDef *>(ADC1_BASE);
inline auto * const ADC2    = reinterpret_cast<ADC_TypeDef *>(ADC2_BASE);
inline auto * const ADC3    = reinterpret_cast<ADC_TypeDef *>(ADC3_BASE);
inline auto * const SDIO    = reinterpret_cast<SDIO_TypeDef *>(SDIO_BASE);
inline auto * const SPI1    = reinterpret_cast<SPI_TypeDef *>(SPI1_BASE);
inline auto * const SYSCFG  = reinterpret_cast<SYSCFG_TypeDef *>(SYSCFG_BASE);
inline auto * const EXTI    = reinterpret_cast<EXTI_TypeDef *>(EXTI_BASE);
inline auto * const TIM9    = reinterpret_cast<TIM_TypeDef *>(TIM9_BASE);
inline auto * const TIM10   = reinterpret_cast<TIM_TypeDef *>(TIM10_BASE);
inline auto * const TIM11   = reinterpret_cast<TIM_TypeDef *>(TIM11_BASE);
inline auto * const GPIOA   = reinterpret_cast<GPIO_TypeDef *>(GPIOA_BASE);
inline auto * const GPIOB   = reinterpret_cast<GPIO_TypeDef *>(GPIOB_BASE);
inline auto * const GPIOC   = reinterpret_cast<GPIO_TypeDef *>(GPIOC_BASE);
inline auto * const GPIOD   = reinterpret_cast<GPIO_TypeDef *>(GPIOD_BASE);
inline auto * const GPIOE   = reinterpret_cast<GPIO_TypeDef *>(GPIOE_BASE);
inline auto * const GPIOF   = reinterpret_cast<GPIO_TypeDef *>(GPIOF_BASE);
inline auto * const GPIOG   = reinterpret_cast<GPIO_TypeDef *>(GPIOG_BASE);
inline auto * const GPIOH   = reinterpret_cast<GPIO_TypeDef *>(GPIOH_BASE);
inline auto * const GPIOI   = reinterpret_cast<GPIO_TypeDef *>(GPIOI_BASE);
inline auto * const CRC     = reinterpret_cast<CRC_TypeDef *>(CRC_BASE);
inline auto * const RCC     = reinterpret_cast<RCC_TypeDef *>(RCC_BASE);
inline auto * const FLASH   = reinterpret_cast<FLASH_TypeDef *>(FLASH_R_BASE);
inline auto * const DMA1    = reinterpret_cast<DMA_TypeDef *>(DMA1_BASE);
inline auto * const DMA1_Stream0 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA1_Stream0_BASE);
inline auto * const DMA1_Stream1 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA1_Stream1_BASE);
inline auto * const DMA1_Stream2 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA1_Stream2_BASE);
inline auto * const DMA1_Stream3 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA1_Stream3_BASE);
inline auto * const DMA1_Stream4 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA1_Stream4_BASE);
inline auto * const DMA1_Stream5 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA1_Stream5_BASE);
inline auto * const DMA1_Stream6 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA1_Stream6_BASE);
inline auto * const DMA1_Stream7 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA1_Stream7_BASE);
inline auto * const DMA2 = reinterpret_cast<DMA_TypeDef *>(DMA2_BASE);
inline auto * const DMA2_Stream0 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA2_Stream0_BASE);
inline auto * const DMA2_Stream1 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA2_Stream1_BASE);
inline auto * const DMA2_Stream2 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA2_Stream2_BASE);
inline auto * const DMA2_Stream3 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA2_Stream3_BASE);
inline auto * const DMA2_Stream4 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA2_Stream4_BASE);
inline auto * const DMA2_Stream5 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA2_Stream5_BASE);
inline auto * const DMA2_Stream6 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA2_Stream6_BASE);
inline auto * const DMA2_Stream7 =
    reinterpret_cast<DMA_Stream_TypeDef *>(DMA2_Stream7_BASE);
inline auto * const ETH  = reinterpret_cast<ETH_TypeDef *>(ETH_BASE);
inline auto * const DCMI = reinterpret_cast<DCMI_TypeDef *>(DCMI_BASE);
inline auto * const CRYP = reinterpret_cast<CRYP_TypeDef *>(CRYP_BASE);
inline auto * const HASH = reinterpret_cast<HASH_TypeDef *>(HASH_BASE);
inline auto * const RNG  = reinterpret_cast<RNG_TypeDef *>(RNG_BASE);
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
}  // namespace sjsu::stm32f4xx

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
