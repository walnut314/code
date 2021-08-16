/******************************************************************************
 *
 * Default Linker Command file for the Texas Instruments LM3S5791
 *
 * This is part of revision 8132 of the Stellaris Peripheral Driver Library.
 *
 * http://www.ti.com/product/LM3S5791
 *
 * 32-bit ARM® Cortex-M3 80-MHz processor core with System Timer (SysTick), 
 * integrated Nested Vectored Interrupt Controller (NVIC), Memory Protection 
 * Unit (MPU), System Control Block (SCB) and Thumb-2 
 * instruction set
 * 
 * On-chip memory, featuring: 128 KB single-cycle Flash up to 50 MHz (a prefetch 
 * buffer improves performance above 50 MHz); 64 KB single-cycle SRAM; internal 
 * ROM loaded with StellarisWare® software
 *
 * 64 KB SRAM
 * 128 KB Flash
 *
 *****************************************************************************/

--retain=g_pfnVectors

MEMORY
{
    /* Use the line comment out to fill the unused memory with zeros        */
    /* FLASH (RX) : origin = 0x00000000, length = 0x00020000, fill = 0x0000 */
    FLASH (RX) : origin = 0x00000000, length = 0x00020000
    SRAM (RWX) : origin = 0x20000000, length = 0x00010000
}

/* The following command line options are set as part of the CCS project.    */
/* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */
/* --library=rtsv7M3_T_le_eabi.lib                                           */

/* Section allocation in memory */

SECTIONS
{
    .intvecs:   > 0x00000000
    .text   :   > FLASH
    .const  :   > FLASH
    .cinit  :   > FLASH
    .pinit  :   > FLASH

    .vtable :   > 0x20000000
    .data   :   > SRAM
    .bss    :   > SRAM
    .sysmem :   > SRAM
    .stack  :   > SRAM
}

__STACK_TOP = __stack + 256;
