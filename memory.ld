MEMORY
{
	FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
	RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

_reset_stack_pointer = ORIGIN(RAM) + LENGTH(RAM);

/* specifies binary layout 
*	a dot (.) specifies the current address
*	so . = 0 puts us at the beginningo of the section
*	data from PM0056 programming manual
*/
SECTIONS
{
	.text : {
		/* this puts us at the beginning of the instrs. (.text)section */
		. = 0;
		LONG(_reset_stack_pointer);
		/* we put the address of the main function here */
		LONG(main | 1);
		/* interrupt table is 67 interrupt vectors from 0x40 increasing
		   in steps of 4 (0x40 = d64 + 67x4 = 332 */
		. = 332;
		 /* rest of the code goes here? */
		*(.text*)
	} > FLASH = 0xFF
}
