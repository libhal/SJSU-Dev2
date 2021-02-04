/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2018
 * Generated linker script file for LPC4078
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.2 [Build 650] [2016-09-09]  on Jun 3, 2018 12:50:46 AM
 */

MEMORY
{
  /* Define each memory region */
  /* 512kB (alias Flash) */
  FLASH (rx) : ORIGIN = 0, LENGTH = 512K
  /* 64kB (alias RAM) */
  SRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0x10000
  /* 32kB (alias RAM2) */
  SRAM_AHB (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000
}

  /* Define a symbol for the top of each memory region */
  __base_FLASH = 0x0  ; /* FLASH */
  __base_Flash = 0x0 ; /* Flash */
  __top_FLASH = 0x0 + 0x80000 ; /* 512K bytes */
  __top_Flash = 0x0 + 0x80000 ; /* 512K bytes */
  __base_SRAM = 0x10000000  ; /* SRAM */
  __base_RAM = 0x10000000 ; /* RAM */
  __top_SRAM = 0x10000000 + 0x10000 ; /* 64K bytes */
  __top_RAM = 0x10000000 + 0x10000 ; /* 64K bytes */
  __base_SRAM_AHB = 0x20000000  ; /* SRAM_AHB */
  __base_RAM2 = 0x20000000 ; /* RAM2 */
  __top_SRAM_AHB = 0x20000000 + 0x8000 ; /* 32K bytes */
  __top_RAM2 = 0x20000000 + 0x8000 ; /* 32K bytes */
  __user_heap_base = __base_RAM2 ; /* SJSU-Dev2: RAM2 is solely for heap */

ENTRY(ArmResetHandler)

SECTIONS
{
    /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ;
        section_table_start = .;
        data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        data_section_table_end = .;
        bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        bss_section_table_end = .;
        section_table_end = . ;
	    /* End of Global Section Table */

        *(.after_vectors*)

        /* Set Code Read Protection data to INVALID so it is ignored */
        . = 0x000002FC;
        LONG(0xFFFFFFFF);
        /* End of Code Read Protection */

        /* C++ constructors etc */
		. = ALIGN(4);
		KEEP(*(.init))

		. = ALIGN(4);
		__preinit_array_start = .;
		KEEP (*(.preinit_array))
		__preinit_array_end = .;

		. = ALIGN(4);
		__init_array_start = .;
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		__init_array_end = .;

		KEEP(*(.fini));

		. = ALIGN(4);
		KEEP (*crtbegin.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*crtend.o(.ctors))

		. = ALIGN(4);
		KEEP (*crtbegin.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*crtend.o(.dtors))
        /* End C++ */
    } >FLASH

    .text : ALIGN(4)
    {
        *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    } > FLASH
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    _etext = .;

    /* DATA section for SRAM_AHB */
    .data_RAM2 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM2 = .) ;
        *(.ramfunc.$RAM2)
        *(.ramfunc.$SRAM_AHB)
        *(.data.$RAM2*)
        *(.data.$SRAM_AHB*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM2 = .) ;
     } > SRAM_AHB AT>FLASH

    /* MAIN DATA SECTION */
    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > SRAM
    /* Main DATA section (SRAM) */
    .data : ALIGN(4)
    {
       FILL(0xff)
       _data = . ;
       *(vtable)
       *(.ramfunc*)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
    } > SRAM AT>FLASH
    /* BSS section for SRAM_AHB */
    .bss_RAM2 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM2 = .) ;
       *(.bss.$RAM2*)
       *(.bss.$SRAM_AHB*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM2 = .) ;
    } > SRAM_AHB
    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > SRAM
    /* NOINIT section for SRAM_AHB */
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM2*)
       *(.noinit.$SRAM_AHB*)
       . = ALIGN(4) ;
    } > SRAM_AHB
    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*)
         . = ALIGN(4) ;
        _end_noinit = .;
    } > SRAM

    PROVIDE(heap = DEFINED(__user_heap_base) ? __user_heap_base : .);
    PROVIDE(heap_end = __top_RAM2);
    PROVIDE(StackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_SRAM - 0);
}