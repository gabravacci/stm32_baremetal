/* this serves as an entry point for debuggers which use the .ELF header */
ENTRY(_reset);

MEMORY
{
  flash(rx)  : ORIGIN = 0x08000000, LENGTH = 64k
  sram(rwx)  : ORIGIN = 0x20000000, LENGTH = 20k
}

_estack     = ORIGIN(sram) + LENGTH(sram);    /* stack points to end of SRAM */

/*
* SECTIONS defines output sections and their order in th .ELF file
* An important distinction must be made here between VMA and LMA.
*
* VMA is where the _program_ expects to find the section at _runtime_. The VM Address is what is
* used by pointers, the program counter, etc. 
*
* LMA is where the _loader_ must place the code. These often coincide but not necessarily.
*/
SECTIONS 
{
  /* we're loading all of this to (physical) flash memory first */
  .vectors  : { KEEP(*(.vectors)) }   > flash
  .text     : { *(.text*) }           > flash
  .rodata   : { *(.rodata*) }         > flash

  .data :
  {
    /*
    * "wild card" operator "*(.x)" means "use the .x section of all files"
    * similarly, ".y*" means use  all files starting with "y"
    * so "*(.text*)" includes ".text.foo", ".text.bar" and so on
    */
    _sdata = .;  /* "." := current location, so in C "_sdata" will be a pointer to here! */
    *(.first_data)
    *(.data SORT(.data.*))
    _edata = .;
    /* (vma) AT > (lma) [???] */
  } > sram AT > flash
  /* "LOADADDR" returns the LMA (duh) of the passed section */
  _sidata = LOADADDR(.data);

  .bss : {
    _sbss = .;              /* .bss section start */
    *(.bss SORT(.bss.*) COMMON)
    _ebss = .;              /* .bss section end */
  } > sram

  . = ALIGN(8);
  _end = .;     /* for cmsis_gcc.h  [???] */
}
