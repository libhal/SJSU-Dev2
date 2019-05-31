/*
 * Copy the linker script from the vendor into this file.
 * Application memory should be offset in flash by 64kB to accomidate the
 * bootloader
 *
 * Modify it slightly to incorporate the following:
 */

/*
. = ALIGN(4) ;
section_table_start = .;

data_section_table = .;
LONG(LOADADDR(.data));
LONG(    ADDR(.data));
LONG(  SIZEOF(.data));
# Add more if you have more then 1 ram block that is not contiguous
LONG(LOADADDR(.data_RAMx));
LONG(    ADDR(.data_RAMx));
LONG(  SIZEOF(.data_RAMx));
data_section_table_end = .;

bss_section_table = .;
LONG(    ADDR(.bss));
LONG(  SIZEOF(.bss));
# Add more if you have more then 1 ram block that is not contiguous
LONG(    ADDR(.bss_RAMx));
LONG(  SIZEOF(.bss_RAMx));
bss_section_table_end = .;

section_table_end = . ;

*/