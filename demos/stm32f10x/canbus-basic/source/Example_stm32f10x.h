/**
 * STM32 Header File
 */

#include <stdint.h>

#ifndef STM32F10X_H
#define STM32F10X_H

/**
 * Struct declarations for each register.
 * Even though the functions tested will only modify and use CAN_Mock,
 * any struct referenced to in the source file must be declared here.
 * 
 */
 
typedef struct
{
	uint32_t EVCR;
	uint32_t MAPR;
	uint32_t EXTICR[4];
	uint32_t RESERVED0;
	uint32_t MAPR2;  
} AFIO_Mock;
 
typedef struct
{
	uint32_t APB1ENR;
	uint32_t APB2ENR;
} RCC_Mock;

typedef struct
{
	uint32_t CRL;
	uint32_t CRH;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSRR;
	uint32_t BRR;
} GPIO_Mock;

typedef struct
{
	uint32_t TIR;
	uint32_t TDTR;
	uint32_t TDLR;
	uint32_t TDHR;
} CAN_TxMailBox_TypeDef;
  
typedef struct
{
	uint32_t RIR;
	uint32_t RDTR;
	uint32_t RDLR;
	uint32_t RDHR;
} CAN_FIFOMailBox_TypeDef;

typedef struct
{
	uint32_t FR1;
	uint32_t FR2;
} CAN_FilterRegister_TypeDef;

typedef struct
{
	CAN_TxMailBox_TypeDef      sTxMailBox[3];
	CAN_FIFOMailBox_TypeDef    sFIFOMailBox[2];
	CAN_FilterRegister_TypeDef sFilterRegister[14];
	
	uint32_t MCR;
	uint32_t MSR;
	uint32_t TSR;
	uint32_t RF0R;
	uint32_t RF1R;
	uint32_t IER;
	uint32_t ESR;
	uint32_t BTR;
	uint32_t FMR;
	uint32_t FM1R;
	uint32_t FS1R;
	uint32_t FFA1R;
	uint32_t FA1R;
} CAN_Mock;

// Required register declarations in place of register macros
extern AFIO_Mock* AFIO;
extern RCC_Mock*  RCC;
extern GPIO_Mock* GPIOB;
extern CAN_Mock*  CAN1;

#endif
