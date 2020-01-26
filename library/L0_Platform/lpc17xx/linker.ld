MEMORY
{
  FLASH (rx) : ORIGIN = 0, LENGTH = 512K
  SRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 32K
  SRAM_AHB (rwx) : ORIGIN = 0x2007c000, LENGTH = 32K
}

/* Define a symbol for the top of each memory region */
__base_Flash = 0x0 ;               /* Flash */
__top_Flash = 0x0 + 0x80000 ;      /* 512K bytes */
__base_RAM = 0x10000000 ;          /* RAM */
__top_RAM = 0x10000000 + 32K ;     /* 32K bytes */
__base_RAM2 = 0x2007c000 ;         /* RAM2 */
__top_RAM2 = 0x2007c000 + 0x8000 ; /* 32K bytes */
__user_heap_base = __base_RAM2 ;   /* SJSU-Dev2: RAM2 is solely for heap */

/* Program entry point */
ENTRY(ArmResetHandler)

SECTIONS
{
	/* MAIN TEXT SECTION */
	.text : ALIGN(4)
	{
		FILL(0xff)
		KEEP(*(.isr_vector))

		/* Global Section Table */
		. = ALIGN(4) ;
		section_table_start = .;
		data_section_table = .;
		LONG(LOADADDR(.data));
		LONG(    ADDR(.data));
		LONG(  SIZEOF(.data));
		data_section_table_end = .;
		bss_section_table = .;
		LONG(    ADDR(.bss));
		LONG(  SIZEOF(.bss));
		bss_section_table_end = .;
		section_table_end = . ;
		/* End of Global Section Table */

    /* Functions that are placed after the interrupt vector */
		*(.after_vectors*)

		*(.text*)
		*(.rodata .rodata.*)
		. = ALIGN(4);

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

		. = ALIGN(4) ;
		__symbol_table_start = .;
        KEEP(*(.symbol_table))
        __symbol_table_end = .;
		/* End C++ */
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

	/* MAIN DATA SECTION */
	.uninit_RESERVED : ALIGN(4)
	{
		KEEP(*(.bss.$RESERVED*))
	} > SRAM

	.data : ALIGN(4)
	{
		FILL(0xff)
		_data = .;
		*(vtable)
		*(.data*)
		. = ALIGN(4) ;
		_edata = .;
	} > SRAM AT>FLASH

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

	/* Provide a symbol of the heap pointer to C/C++ code */
  PROVIDE(heap = DEFINED(__user_heap_base) ? __user_heap_base : .);
  PROVIDE(heap_end = __top_RAM2);
	PROVIDE(StackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RAM);
}
