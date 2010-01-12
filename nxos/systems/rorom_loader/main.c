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
#include "base/drivers/usb.h"

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

typedef struct {
    U8 *start_address;
    S32 length;
} tx_data_t;

static
tx_data_t read_tx_data()
{
    U32 read = 0;
    S32 remains = sizeof(tx_data_t);
    tx_data_t txd;

    memset(&txd, 0, remains);
    while (remains > 0) {
        nx_usb_read((void *)&txd, remains);
        while ((read = nx_usb_data_read()) == 0);
        remains -= read;
    }
    return txd;
}

void main (void)
{
    tx_data_t txd;
    S32 read;
    U8 buffer[NX_USB_PACKET_SIZE];

    nx_systick_install_scheduler(security_hook);
    nx_display_string("Hello world");

    // Wait for USB connection
    while (nx_usb_is_connected());

    txd = read_tx_data();
    nx_display_string("\n  addr=0x");
    nx_display_hex((unsigned)txd.start_address);
    nx_display_string("\n  len=0x");
    nx_display_uint(txd.length);

    while (txd.length > 0) {
        nx_usb_read((void *)buffer, NX_USB_PACKET_SIZE);
        while ((read = nx_usb_data_read()) == 0);
        read = read < txd.length ? read : txd.length;
        txd.length -= read;
        nx_display_cursor_set_pos(0, 4);
        nx_display_string("\nread=0x        ");
        nx_display_cursor_set_pos(7, 4);
        nx_display_uint(read);
        nx_display_string("\ncurs=0x        ");
        nx_display_cursor_set_pos(7, 5);
        nx_display_hex((U32)txd.start_address);
        nx_display_string("\nleft=0x        ");
        nx_display_cursor_set_pos(7, 6);
        nx_display_uint(txd.length);
        memcpy(txd.start_address, buffer, read);
        txd.start_address += read;
    }
    nx_display_clear();
    nx_display_string("\nOK. Now load RAM part.");
    for(;;);
}

