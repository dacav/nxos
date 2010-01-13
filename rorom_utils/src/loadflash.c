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
    if (argc < 2) {
        fprintf(stderr, "Provide the ROM image as first parameter.\n");
        exit(1);
    }

    const char *filename = argv[1];
    // FIXME this should be parametrized!
    tx_data_t data = { .start_address = 0x100000 + 0x8000 };

    if ((data.length = get_ro_size(filename)) == -1) {
        fprintf(stderr, "Invalid ROM image\n");
        exit(1);
    }
    
    nxt_link_t nxt;
    uint8_t buffer[USB_BUFSIZE];
    size_t read, sent;

    if (nxt_init(&nxt)) {
        fprintf(stderr, "Initialization failed\n");
        exit(1);
    }

    FILE *ro_data = fopen(filename, "rb");
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

    while ((read = fread((void *)buffer, USB_BUFSIZE, 1, ro_data)) > 0) {
        read *= USB_BUFSIZE;
        sent = nxt_send(&nxt, (void *)buffer, read);
        fprintf(stdout, "Remaining: %-20d\r", data.length);
        if (sent == -1) {
            fprintf(stderr, "Send failed: %s\n", nxt_libusb_strerr(&nxt));
            break;
        }
        data.length -= sent;
        data.start_address += sent;
        usleep(DELAY_uSECS);
    }
    fprintf(stdout, "\n");

    if (ferror(ro_data)) {
        fprintf(stderr, "File error in %s\n", filename);
    }

    fclose(ro_data);
    nxt_free(&nxt);
    exit(0);
}

