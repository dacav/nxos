#include "./record.h"
#include "base/types.h"
#include "base/drivers/usb.h"

#define VECTOR_LEN 32

struct act_rec {
  U8 vector[VECTOR_LEN];  /* Guest interrupt vector, to load at RAM init */
  struct {
    U32 activation;     /* Guest activation address */
    U32 sec_data;       /* Section .data */
    U32 sec_bss;        /* Section .bss */
    U32 sec_stacks;     /* Modes stacks */
  } addr;
};

static inline void wait_connection(void) {
  while (!nx_usb_is_connected());
}

#include "base/display.h"

bool new_guest(void) {
  U8 buffer[NX_USB_PACKET_SIZE];
  struct act_rec *rec;

  rec = (struct act_rec *)buffer;
  wait_connection();
  nx_usb_read(buffer, NX_USB_PACKET_SIZE);

  while (nx_usb_data_read() < sizeof(struct act_rec));

  nx_display_string("Activ: 0x");
  nx_display_hex(rec->addr.activation);
  nx_display_string("\n.data: 0x");
  nx_display_hex(rec->addr.sec_data);
  nx_display_string("\n.bss: 0x");
  nx_display_hex(rec->addr.sec_bss);
  nx_display_string("\n.stacks: 0x");
  nx_display_hex(rec->addr.sec_stacks);

  return TRUE;
}

