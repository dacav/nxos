/** Tag-route follower.
 *
 * Replays a recorded tag route.
 */

#include "base/types.h"
#include "base/core.h"
#include "base/display.h"
#include "base/drivers/avr.h"
#include "base/drivers/systick.h"

void main(void)
{
    nx_display_string("Hello world\n");
    for (;;);
}
