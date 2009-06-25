/* Copyright (c) 2007,2008 the NxOS developers
 *
 * See AUTHORS for a full list of the developers.
 *
 * Redistribution of this file is permitted under
 * the terms of the GNU Public License (GPL) version 2.
 */

/* BOATLOODER linker script
 *
 * This file contains the linker script suitable for a boatlooder loadable
 * binary image of NXOS
 *
 *  Notes on alignment:
 *  1) Sections should be 4-byte aligned otherwise ARM fetches will be
 *     misaligned.
 *  2) The stack should be 8-byte aligned for the APCS. That's because
 *     STRD and LDRD assume that they are on 8-byte boundaries.
 */

/*
 * Memory definitions.
 */

MEMORY {

    /* Second page of ROM */
    rom :           ORIGIN = 1M + 128,  LENGTH = 256k

    vector_ram :    ORIGIN = 2M,        LENGTH = 52
    ram :           ORIGIN = 2M + 52,   LENGTH = 64K - 52

    load_info :     ORIGIN = 0,         LENGTH = 1M
}

ROM_BASE = 1M;
ROM_SIZE = 256k;
RAM_BASE = 2M;
RAM_SIZE = 64k;

SECTIONS {

    /* Padding for interrupt vector: 52 bytes + align padding */
    .vectors : {
        LONG(0); LONG(0); LONG(0); LONG(0);
        LONG(0); LONG(0); LONG(0); LONG(0);
        LONG(0); LONG(0); LONG(0); LONG(0);
        LONG(0);
        . = ALIGN(4);
    } > vector_ram

    /* Boatlooder information record */
    .boatlooder : {
        . = ALIGN(0);
        KEEP(base/vectors.o (*.text *.text.*))
        . = ALIGN(0);
    } > load_info
 
    /*
     * This section contains code that is relocated to RAM before
     * execution.
     *
     * This section is used for code that has to reside in ram to
     * function correctly (like flash writing code)
     */
    .ram_text : {
        . = ALIGN(4);
        *.oram (*.text *.text.* *.glue*)
        . = ALIGN(4);
        *.oram (*.rodata *.rodata.*)
    }   > ram
      AT> rom

    /*
     * Read/Write initialized data. Runs from RAM, gets copied there
     * from ROM if booting from flash.
     */
    .data : {
        . = ALIGN(4);
        * (*.data *.data.*)
    }   > ram
      AT> rom

    /*
     * The main kernel executable code
     */
    .text : {
        . = ALIGN(4);
        * (*.text *.text.* *.glue*)
    }   > rom

    /*
     * Read only data
     */
    .rodata : {
        . = ALIGN(4);
        * (*.rodata *.rodata.*)
    }   > rom

    /*
     * bss section 
     */
    .bss : {
        . = ALIGN(4);
        * (*.bss *.bss.*)
     }  > ram
      AT> rom

    /*
     * The various kernel stacks.
     *
     * Although not strictly required, this zone is zeroed out at boot
     * time to provide a clean environment for the kernel.
     */
    .stack  : {
        /* irq stack */
        . = ALIGN(8);
        . += 0x60; /* 96b, which is 3*8*4, the maximum irq stack size. */
        . = ALIGN(8);
        __irq_stack__ = .;

        /* supervisor stack */
        __supervisor_stack_bottom__ = . ;
        . += 0x400; /* 1k supervisor stack. */
        . = ALIGN(8);
        __supervisor_stack__ = .;
        __supervisor_stack_top__ = . ;
    } > ram

    /* Discard anything not mapped at this point. */
    /DISCARD/ : {
        * (*);
    }


    /*
    * Symbol definitions for the use of the kernel code.
    */

    __vectors_ram_start__  = ADDR(.vectors);
    __vectors_ram_end__    = (ADDR(.vectors) + SIZEOF(.vectors));
    __vectors_load_start__ = LOADADDR(.vectors);
    __vectors_load_end__   = (LOADADDR(.vectors) + SIZEOF(.vectors));

    __ramtext_ram_start__ = ADDR(.ram_text);
    __ramtext_ram_end__   = ADDR(.ram_text) + SIZEOF(.ram_text);
    __ramtext_load_start__ = LOADADDR(.ram_text);
    __ramtext_load_end__ = __ramtext_load_start__ + SIZEOF(.ram_text) ;

    __data_ram_start__ = ADDR(.data);
    __data_ram_end__   = ADDR(.data) + SIZEOF(.data);
    __data_load_start__ = LOADADDR(.data);
    __data_load_end__ = __data_load_start__ + SIZEOF(.data) ;

    __text_start__ = ADDR(.text);
    __text_end__ = ADDR(.text) + SIZEOF(.text);

    __bss_start__ = ADDR(.bss);
    __bss_end__   = (ADDR(.bss) + SIZEOF(.bss));

    __stack_start__ = ADDR(.stack);
    __stack_end__ = (ADDR(.stack) + SIZEOF(.stack));

/*  SAM_ONLY __boot_from_samba__ = 1;
    ROM_ONLY __boot_from_samba__ = 0;

  ROM_ONLY __rom_userspace_start__ = ADDR(.text) + SIZEOF(.text);
  SAM_ONLY __rom_userspace_start__ = 0;
*/
    __rom_userspace_end__ = (ROM_BASE + ROM_SIZE);

    __ram_userspace_start__ = __stack_end__;
    __ram_userspace_end__ = (RAM_BASE + RAM_SIZE);

/*
  SAM_ONLY __kernel_ram_load_size__ = __text_end__ - __vectors_ram_start__;
*/
}
