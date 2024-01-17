/* Linker script for ST STM32F4DISCOVERY (STM32F407VG, 1024K flash, 128K RAM). */

/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 192K
	ccmram (xrw) : ORIGIN = 0x10000000,   LENGTH = 64K
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld