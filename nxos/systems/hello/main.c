/** Tag-route follower.
 *
 * Replays a recorded tag route.
 */

#include "base/types.h"
#include "base/core.h"
#include "base/display.h"
#include "base/drivers/avr.h"
#include "base/drivers/systick.h"

static void security_hook(void)
{
    if (nx_avr_get_button() == BUTTON_CANCEL)
        nx_core_halt();
}

void main(void)
{
    nx_systick_install_scheduler(security_hook);

    nx_display_string("Hello world\n");
    for (;;);
}
