/* Copyright (c) 2007,2008 the NxOS developers
 *
 * See AUTHORS for a full list of the developers.
 *
 * Redistribution of this file is permitted under
 * the terms of the GNU Public License (GPL) version 2.
 */

#include "base/types.h"
#include "base/display.h"
#include "base/lib/memalloc/memalloc.h"
#include "base/at91sam7s256.h"
#include "base/drivers/avr.h"
#include "base/lib/rcmdlw/commands.h"
#include "base/drivers/systick.h"
#include "base/drivers/sound.h"
#include "base/core.h"
#include "base/drivers/motors.h"
#include "base/drivers/rs485.h"

static 
bool error_mng(nx_rs485_error_t err, char *extmsg) 
{
  const char *msg = "OK";
  bool ret;

  ret = FALSE;
  switch (err) {
  case RS485_SUCCESS:
    ret = TRUE;
    break;
  case RS485_TIMEOUT:
    msg = "Time";
    break;
  case RS485_OVERRUN:
    msg = "Over";
    break;
  case RS485_FRAMING:
    msg = "Fram";
    break;
  case RS485_PARITY:
    msg = "Pari";
    break;
  default:
    msg = "??  ";
  }
  if (!ret) {
    nx_display_string(extmsg);
    nx_display_string(msg);
    nx_display_end_line();
  }
  return ret;
}

static volatile struct {
  nx_rs485_error_t error;
  bool lock;
} cb_info;

static
void done(nx_rs485_error_t err)
{
  cb_info.error = err;
  cb_info.lock = FALSE; 
}

#if 0
static
void dump(U8 *buffer, size_t len, U8 col)
{
  register U8 i;
  static U8 off = 0;

  nx_display_cursor_set_pos(0, col + off);
  for (i=0; i<len; i++) {
    nx_display_hex(buffer[i]);
    nx_display_string(" ");
  }
  off = (off + 1) % 4;
}
#endif

#define USART_MODEREG AT91C_US_USMODE_RS485 | \
                      AT91C_US_CLKS_CLOCK   | \
                      AT91C_US_CHRL_8_BITS  | \
                      AT91C_US_PAR_NONE     | \
                      AT91C_US_OVER         | \
                      AT91C_US_NBSTOP_1_BIT

static
void sync_rs485(void) {
  U8 b = 0;

  nx_display_string("syncing... ");
  cb_info.lock = TRUE;
  nx_rs485_recv(&b, 1, done);
  while (cb_info.lock);

  nx_sound_freq_async(1500, 100);
}

/* Masks stolen from base/lib/rcmdlw/commands.c */

#define MSK_ACTION(cmd) ((cmd) & 0xe0)
#define MSK_MOV_TURN(cmd) ((cmd) & 0x08)
enum {
    RCMD_MOVE = 4 << 5,
    RCMD_STOP = 3 << 5
};

/* This is VERY DIRTY, please don't hate me. */

static const S8 speed = 80;
static const float k = 0.42;
static volatile bool enabled = FALSE;
static U32 cnt = 0;
static void security_hook(void) {
  S32 diff, a, b;

  if (nx_avr_get_button() == BUTTON_CANCEL)
    nx_core_halt();

  cnt++;
  if (cnt & 0x3)
    return;

  if (enabled) {
    a = nx_motors_get_tach_count(0);
    b = nx_motors_get_tach_count(2);
    diff = a - b;
    nx_motors_rotate(0, speed - diff * k);
    nx_motors_rotate(2, speed);
  }
}

static
void receiver(void)
{
  U8 buffer[6];
  bool req_ack = FALSE;
  U8 cmd;

  nx_display_cursor_set_pos(0, 1);
  while (TRUE) {
    cb_info.lock = TRUE;
    nx_rs485_recv(buffer, sizeof(buffer), done);
    while (cb_info.lock);
    if (error_mng(cb_info.error, "r: ") == FALSE)
      continue;
    cmd = buffer[0];
    if (MSK_ACTION(cmd) == RCMD_MOVE && !MSK_MOV_TURN(cmd)) {
      /* Bypassed library */ 
      enabled = TRUE;
    } else {
      if (MSK_ACTION(cmd) == RCMD_STOP) {
        enabled = FALSE;
      }
      nx_cmd_interpret(buffer, sizeof(buffer), &req_ack);
    }
    if (req_ack) {
      while (cb_info.lock);
      nx_rs485_send(buffer, sizeof(buffer), done);
      error_mng(cb_info.error, "s: ");
    }
  }
}

void main(void) {

  nx_systick_install_scheduler(security_hook);

  nx_memalloc_init();
  nx_rs485_init(RS485_BR_9600, USART_MODEREG, 0, FALSE);
  nx_rs485_set_fixed_baudrate(0x259);
  sync_rs485();
  receiver();
}
