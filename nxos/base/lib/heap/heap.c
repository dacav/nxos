#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "heap.h"

void heap_init (heap_t *h, S32 *array, size_t size)
{
    h->values = array;
    h->size = size;
    h->used = 0;
}

U32 heap_insert (heap_t *h, S32 val)
{
    size_t used;
    S32 *values;
   
    used = h->used;
    if (used >= h->size) {
        return 1;
    }

    values = h->values;
    values[used++] = val;
    heapify_up(values, used);
    h->used = used;

    return 0;
}

U32 heap_extract (heap_t *h, S32 *val)
{
    S32 *values;
    size_t used;

    values = h->values;
    used = h->used;
    if (used <= 0) {
        return 1;
    }
    *val = *values;
    heapify_down(values, used);
    h->used = used - 1;

    return 0;
}

