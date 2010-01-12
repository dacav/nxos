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

#include <libusb-1.0/libusb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* Various constants, NXOS related */
#define TX_ENDPOINT 1
#define TX_TIMEOUT  0
#define RX_ENDPOINT 2
#define RX_TIMEOUT  0   
#define USB_BUFSIZE 64  // nxos usb buffer size

/* USB Parameters of NXT/NxOS */
static const unsigned NXT_VENDOR_ID  = 0x0694;
static const unsigned NXT_PRODUCT_ID = 0xff00;

/* USB Parameters of Sam-ba */
static const unsigned SAM_VENDOR_ID  = 0x03eb;
static const unsigned SAM_PRODUCT_ID = 0x6124;

typedef struct {
    libusb_context *context;
    libusb_device_handle *handle;
} nxt_link_t;

static
const char *libusb_get_error (int err)
{
    switch (err) {
        case LIBUSB_SUCCESS:
            return "Success";
        case LIBUSB_ERROR_IO:
            return "I/O error";
        case LIBUSB_ERROR_INVALID_PARAM:
            return "Invalid param";
        case LIBUSB_ERROR_ACCESS:
            return "Access error";
        case LIBUSB_ERROR_NO_DEVICE:
            return "Device not found";
        case LIBUSB_ERROR_NOT_FOUND:
            return "Entity not found";
        case LIBUSB_ERROR_BUSY:
            return "Resource busy";
        case LIBUSB_ERROR_TIMEOUT:
            return "Timed out";
        case LIBUSB_ERROR_OVERFLOW:
            return "Overflow";
        case LIBUSB_ERROR_PIPE:
            return "Pipe error";
        case LIBUSB_ERROR_INTERRUPTED:
            return "Interrupted";
        case LIBUSB_ERROR_NO_MEM:
            return "Not enough memory";
        case LIBUSB_ERROR_NOT_SUPPORTED:
            return "Operation not supported";
        case LIBUSB_ERROR_OTHER:
        default:
            return "Unknown error";
    }
}

static
int usb_init (nxt_link_t *nxt)
{
    libusb_context *context;
    libusb_device_handle *handle;
    int err;

    if ((err = libusb_init(&context)) != LIBUSB_SUCCESS) {
        fprintf(stderr, "LibUsb: %s\n", libusb_get_error(err));
        return 1;
    }
 
    handle = libusb_open_device_with_vid_pid(context, NXT_VENDOR_ID,
             NXT_PRODUCT_ID);
    if (handle == NULL) {
        fprintf(stderr, "Opening NXT: ");
        
        handle = libusb_open_device_with_vid_pid(context, SAM_VENDOR_ID,
                 SAM_PRODUCT_ID);
        if (handle != NULL) { 
            libusb_close(handle);
            libusb_exit(context);
            fprintf(stderr, "First of all load NxOS\n");
        } else {
            fprintf(stderr, "Device not found\n");
        }
        return 1;
    }

    nxt->context = context;
    nxt->handle = handle;
    return 0;
}

static
void usb_free (nxt_link_t *nxt)
{
    libusb_close(nxt->handle);
    libusb_exit(nxt->context);
}

static
int usb_send (nxt_link_t *nxt, void *buffer, size_t len)
{
    int err;
    int32_t transf;

    if ((err = libusb_bulk_transfer(nxt->handle, TX_ENDPOINT, buffer,
                    len, &transf, TX_TIMEOUT)) != LIBUSB_SUCCESS) {
        fprintf(stderr, "Data transfer: %s\n", libusb_get_error(err));
        transf = -1;
    }
    return transf;
}

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
    tx_data_t data = { .start_address = 0x100000 };

    if ((data.length = get_ro_size(filename)) == -1) {
        fprintf(stderr, "Invalid ROM image\n");
        exit(1);
    }
    
    nxt_link_t nxt;
    uint8_t buffer[USB_BUFSIZE];
    size_t read, sent;

    if (usb_init(&nxt)) {
        exit(1);
    }

    FILE *ro_data = fopen(filename, "rb");
    if (ro_data == NULL) {
        fprintf(stderr, "Error in opening %s\n", filename);
        usb_free(&nxt);
    }

    fprintf(stdout, "Sync sent: %d bytes\n"
                    "           start=%p\n"
                    "           length=%d\n",
            usb_send(&nxt, (void *)&data, sizeof(tx_data_t)),
            (void *)data.start_address,
            data.length);

    while ((read = fread((void *)buffer, USB_BUFSIZE, 1, ro_data)) > 0) {
        read *= USB_BUFSIZE;
        sent = usb_send(&nxt, (void *)buffer, read);
        fprintf(stdout, "Remaining: %d\r", data.length);
        if (sent == -1) {
            break;
        }
        data.length -= sent;
        data.start_address += sent;
    }
    fprintf(stdout, "\n");

    if (ferror(ro_data)) {
        fprintf(stderr, "File error in %s\n", filename);
    }

    fclose(ro_data);
    usb_free(&nxt);
    exit(0);
}

