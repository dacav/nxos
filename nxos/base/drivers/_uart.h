#ifndef __NXOS_UART_H__
#define __NXOS_UART_H__

#include "base/types.h"


/***
 *** This driver only support the uart where the bluetooth chip is
 *** connected.
 ***/


/* this macro is just here to see clearly where the debug code is
 */
#define UART_DEBUG

/*
 * called back each time something has been readed.
 * The buffer where the read stuffs are is given in parameter.
 */
typedef void (*uart_read_callback_t)(U8 *buffer, U8 packet_size);


/*
 * the driver using an uart port must configure itself the PIO. The
 * callback will be called by the interruption function of the uart
 * driver, so it must do its work as fast as possible.
 */
void nx_uart_init(uart_read_callback_t callback);

/*
 * Will send the data asap.
 * If the two slot in the pdc are used, this function will be blocking.
 */
void nx_uart_write(void *data, U16 lng);

bool nx_uart_can_write();
bool nx_uart_is_writing();

/* TO REMOVE: */

U32 nx_uart_nmb_interrupt();
U32 nx_uart_get_csr();
U32 nx_uart_get_last_csr();
U32 nx_uart_get_state();

#endif