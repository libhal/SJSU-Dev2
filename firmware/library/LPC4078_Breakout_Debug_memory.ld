/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2018
 * Generated linker script file for LPC4078
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.2 [Build 650] [2016-09-09]  on Jun 3, 2018 12:50:46 AM
 */

MEMORY
{
  /* Define each memory region */
  MFlash512 (rx) : ORIGIN = 0x0, LENGTH = 0x80000 /* 512K bytes (alias Flash) */  
  RamLoc64 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x10000 /* 64K bytes (alias RAM) */  
  RamPeriph32 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32K bytes (alias RAM2) */  
}

  /* Define a symbol for the top of each memory region */
  __base_MFlash512 = 0x0  ; /* MFlash512 */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_MFlash512 = 0x0 + 0x80000 ; /* 512K bytes */  
  __top_Flash = 0x0 + 0x80000 ; /* 512K bytes */  
  __base_RamLoc64 = 0x10000000  ; /* RamLoc64 */  
  __base_RAM = 0x10000000 ; /* RAM */  
  __top_RamLoc64 = 0x10000000 + 0x10000 ; /* 64K bytes */  
  __top_RAM = 0x10000000 + 0x10000 ; /* 64K bytes */  
  __base_RamPeriph32 = 0x20000000  ; /* RamPeriph32 */  
  __base_RAM2 = 0x20000000 ; /* RAM2 */  
  __top_RamPeriph32 = 0x20000000 + 0x8000 ; /* 32K bytes */  
  __top_RAM2 = 0x20000000 + 0x8000 ; /* 32K bytes */  
