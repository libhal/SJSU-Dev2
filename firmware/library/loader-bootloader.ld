MEMORY
{
    /* Flash memory region for the program, offset by the bootloader section */
    FLASH (rx) : ORIGIN = 0K, LENGTH = 512K

    /* 32K (Heap starts here) */
    SRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 64K

    /* 32K (Globals at bottom, stack on top), heap continues from here */
    SRAM_AHB (rwx) : ORIGIN = 0x20000000, LENGTH = 32K

    /* Heap starts at SRAM because if heap starts from higher memory, then
     * malloc tracking routines get confused when sbrk() system function
     * returns higher heap memory first, and then lower heap memory.
     * So we start heap from SRAM, and then move it up to SRAM_AHB if SRAM
     * portion runs out completely.
     */
}

/* Program entry point */
ENTRY(isr_reset)

SECTIONS
{
	/* MAIN TEXT SECTION */
	.text : ALIGN(4)
	{
		FILL(0xff)
		KEEP(*(.isr_vector))

		/* Global Section Table */
		. = ALIGN(4) ;
		__section_table_start = .;
		__data_section_table = .;
		LONG(LOADADDR(.data));
		LONG(    ADDR(.data));
		LONG(  SIZEOF(.data));
		__data_section_table_end = .;
		__bss_section_table = .;
		LONG(    ADDR(.bss));
		LONG(  SIZEOF(.bss));
		__bss_section_table_end = .;
		__section_table_end = . ;
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

/*
	.debug : ALIGN(4)
    {
        *(.debug* *debug.*)
    } > FLASH

    .stabs : ALIGN(4)
    {
        *(.stabs* *stabs.*)
    } > FLASH
*/

	_etext = .;

	/* MAIN DATA SECTION */
	.uninit_RESERVED : ALIGN(4)
	{
		KEEP(*(.bss.$RESERVED*))
	} > SRAM_AHB

	.data : ALIGN(4)
	{
		FILL(0xff)
		_data = .;
		*(vtable)
		*(.data*)
		. = ALIGN(4) ;
		_edata = .;
	} > SRAM_AHB AT>FLASH

	/* MAIN BSS SECTION */
	.bss : ALIGN(4)
	{
		_bss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4) ;
		_ebss = .;
		PROVIDE(end = .);
	} > SRAM_AHB

	/*
	.symbol_table : ALIGN(4)
    {
        __symbol_table_start = .;
        KEEP(*(.symbol_table))
        __symbol_table_end = .;
    } > FLASH_SYMBOL_TABLE
	*/

	/* Provide a symbol of the heap pointer to C/C++ code */
	PROVIDE(_pvHeapStart = .);

	/* Provide the start of the initial stack pointer
	 * Debugger and ISP may use 32-bytes of space?
	 */
	PROVIDE(_vStackTop = ORIGIN(SRAM_AHB) + LENGTH(SRAM_AHB) - 32);
}
