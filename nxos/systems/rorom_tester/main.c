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

#include "base/drivers/_efc.h"
#include "base/lib/bitmap/bitmap.h"

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

static const unsigned PAGE_MAX      = 1024;
static const unsigned PAGE_SIZE_8   = 256;
static const unsigned PAGE_SIZE_32  = 64;

void main (void)
{
    nx_systick_install_scheduler(security_hook);

    register U32 i, j;
    nx_bmp_t map = 0;

    j = 1;
    nx__efc_init();
    for (i = 0; i < PAGE_MAX; i ++) {
        if (nx__efc_erase_page(i, 0)) {
            nx_bmp_set(&map, j);
        }
        j = (j + 1) % 33;
        if (!j) {
            j ++;
            nx_display_hex(map);
            nx_bmp_set(&map, 0);
            nx_systick_wait_ms(500);
        }
    }
}
