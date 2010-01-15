/* Copyright (c) 2008 the NxOS developers
 *
 * See AUTHORS for a full list of the developers.
 *
 * Redistribution of this file is permitted under
 * the terms of the GNU Public License (GPL) version 2.
 */

#include "base/core.h"
#include "base/util.h"
#include "base/display.h"
#include "base/drivers/systick.h"
#include "base/drivers/avr.h"

/* The security hook lets the tester shut down the brick despite the
 * main thread of execution being locked up due to a bug. As long as
 * the system timer and AVR link are working, pressing the Cancel
 * button should power off the brick.
 */
static void security_hook(void)
{
    if (nx_avr_get_button() == BUTTON_CANCEL)
        nx_core_halt();
}

static const U32 * CODE_START = (U32 *)0x108000;
static const U32 * MAX_ROM    = (U32 *)0x140000;

void main (void)
{
    nx_systick_install_scheduler(security_hook);

    register U32 const *i;
    for (i = CODE_START; i < MAX_ROM; i ++) {
        nx_display_hex((U32)i);
        nx_display_string(" ");
        nx_display_hex(*i);
        nx_display_string("\n");
        nx_systick_wait_ms(2000);
    }
}
