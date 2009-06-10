#include "base/display.h"
#include "base/drivers/avr.h"
#include "base/core.h"
#include "base/drivers/systick.h"

#include "./record.h"

static void watchdog(void) {
 if (nx_avr_get_button() == BUTTON_CANCEL)
    nx_core_halt();
}

void main(void) {
  nx_systick_install_scheduler(watchdog);
  new_guest();
  while(1);
}

