/** @file gcc_defines.h
 *  @brief GCC specific definitions and attributes.
 *
 * This definitions allow to declare attributes for functions and
 * variables.
 */

/* Copyright (c) 2007,2008,2009,2010 the NxOS developers
 *
 * See AUTHORS for a full list of the developers.
 *
 * Redistribution of this file is permitted under
 * the terms of the GNU Public License (GPL) version 2.
 */

#ifndef __NXOS_BASE_GCC_DEFINES__
#define __NXOS_BASE_GCC_DEFINES__

/** @addtogroup kernelinternal */
/*@{*/

/** Declares a function as belonging to the .oram ELF section
 *
 * This is useful for code (like efc drivers) which must be located in
 * RAM.
 *
 * Example of usage:
\verbatim
    int RAMCODE foo (int bar) { ... }
\endverbatim
 * 
 */
#define RAMCODE __attribute__((section(".oram")))

/*@}*/

#endif /* __NXOS_BASE_GCC_DEFINES__ */
