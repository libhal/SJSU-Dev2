MEMORY
{
  FLASH      (rx) : ORIGIN = 0x00000000, LENGTH = 256K
  FLASH_INFO (rx) : ORIGIN = 0x00200000, LENGTH = 16K
  /* From data-sheet: "The SRAM is aliased in both Code and SRAM zones." */
  SRAM_CODE (rwx) : ORIGIN = 0x01000000, LENGTH = 64K
  SRAM_DATA  (rw) : ORIGIN = 0x20000000, LENGTH = 64K
  /*
  SRAM_CODE and SRAM_DATA should be aliased.
  ALIAS
  {
    SRAM_CODE (rwx) : ORIGIN = 0x01000000
    SRAM_DATA  (rw) : ORIGIN = 0x20000000
  } LENGTH = 0x0001000
  /* 32KB SRAM region in Code Zone */
  /* 32KB SRAM region in SRAM Zone */
}
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
    . = ALIGN(4);
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

    . = ALIGN(4);
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
  } > SRAM_CODE
  /* Main DATA section (SRAM) */
  .data : ALIGN(4)
  {
    FILL(0xff)
    _data = .;
    *(vtable)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > SRAM_CODE AT > FLASH
  /* MAIN BSS SECTION */
  .bss : ALIGN(4)
  {
    _bss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    PROVIDE(end = .);
  } > SRAM_CODE

  PROVIDE(heap = .);
  PROVIDE(heap_end = ORIGIN(SRAM_CODE) + LENGTH(SRAM_CODE) - 32);
  PROVIDE(StackTop = ORIGIN(SRAM_CODE) + LENGTH(SRAM_CODE) - 32);
}
