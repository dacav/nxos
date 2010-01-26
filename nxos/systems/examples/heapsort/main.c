#include "base/lib/heap/heap.h"
#include "base/display.h"
#include "base/types.h"

void main (void)
{
    heap_t h;
    S32 array[] = {0, 0, 0, 0, 0, 0};
    S32 value;

    heap_init(&h, array, sizeof(array) >> 2);
    heap_insert(&h, 30);
    heap_insert(&h, 20);
    heap_insert(&h, 40);
    heap_insert(&h, 10);
    heap_insert(&h, 50);
    heap_insert(&h, 0);

    while(heap_extract(&h, &value) == 0) {
        nx_display_uint(value);
        nx_display_end_line();
    }

    for(;;);
}
