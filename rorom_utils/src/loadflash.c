/*
 * Copyright 2009 Giovanni Simoni
 *
 * This file is part of RoRom Utils.
 *
 * RoRom Utils is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RoRom Utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RoRom Utils.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "nxt.h"

#ifndef DELAY_uSECS
#define DELAY_uSECS 50000
#endif

static
int get_ro_size (const char *path)
{
    struct stat buf;
    if (stat(path, &buf) == -1) {
        return -1;
    } else {
        return buf.st_size;
    }
}

typedef struct {
    uint32_t start_address;
    int32_t length;
} tx_data_t;

int main (int argc, char **argv)
{
    const char *filename;
    tx_data_t data;
    nxt_link_t nxt;
    uint8_t buffer[USB_BUFSIZE];
    size_t read, sent;
    FILE *ro_data;

    if (argc < 2) {
        fprintf(stderr, "Provide the ROM image as first parameter.\n");
        exit(1);
    }
    filename = argv[1];

    // FIXME this should be parametrized. Basically it's the address of
    // the first unlocked flash memory location.
    data.start_address = 0x108000;

    if ((data.length = get_ro_size(filename)) == -1) {
        fprintf(stderr, "Invalid ROM image\n");
        exit(1);
    }
    
    if (nxt_init(&nxt)) {
        fprintf(stderr, "Initialization failed\n");
        exit(1);
    }

    ro_data = fopen(filename, "rb");
    if (ro_data == NULL) {
        fprintf(stderr, "Error in opening %s\n", filename);
        nxt_free(&nxt);
    }

    fprintf(stdout, "Sync sent: %d bytes\n"
                    "           start=%p\n"
                    "           length=%d\n",
            nxt_send(&nxt, (void *)&data, sizeof(tx_data_t)),
            (void *)data.start_address,
            data.length);

    while ((read = fread((void *)buffer, 1, USB_BUFSIZE, ro_data)) > 0) {
        sent = nxt_send(&nxt, (void *)buffer, read);
        fprintf(stdout, "Sent: %-8d | ", sent);
        data.length -= sent;
        fprintf(stdout, "Remaining: %-8d | ", data.length);
        data.start_address += sent;
        fprintf(stdout, "Position: %p | ", (void *)data.start_address);
        if (sent == -1) {
            fprintf(stderr, "Send failed: %s\r", nxt_libusb_strerr(&nxt));
            break;
        } else {
            fprintf(stdout, "Send achieved.\r");
        }
        usleep(DELAY_uSECS);
    }
    fprintf(stdout, "\nCompleted\n");

    if (ferror(ro_data)) {
        fprintf(stderr, "File error in %s\n", filename);
    }

    fclose(ro_data);
    nxt_free(&nxt);
    exit(0);
}

