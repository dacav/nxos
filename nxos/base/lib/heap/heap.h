/** @file heap.h
 *  @brief Heap management.
 */

/* Copyright (C) 2010 Giovanni [dacav] Simoni
 *
 * This portion of code has been developed indipendently and subsequently
 * it has been integrated into NxOS.
 *
 * Redistribution of this file is permitted under
 * the terms of the GNU Public License (GPL) version 3.
 */

#ifndef __NXOS_HEAP__
#define __NXOS_HEAP__

#include "base/types.h"

/** @addtogroup lib */
/*@{*/

/** Heap for storage of integer. */
typedef struct {
    size_t size;    /**< Number of available slots */
    size_t used;    /**< Number of used slots */
    S32 *values;    /**< Array of values */
} heap_t;

/** Initialization procedure.
 *
 * Simply inserts array and size in the correct place and set used to
 * zero. The array must be already allocated
 *
 * @param h The heap structure;
 * @param array The array to be tracked;
 * @param size The size.
 */
void heap_init (heap_t *h, S32 *array, size_t size);

/** Insert procedure.
 *
 * Insert one element in the first position of the heap_t structure, than
 * run heapify_up over the values array.
 *
 * @param h The heap;
 * @param val The value to be inserted;
 * @return 0 on success, 1 on failure (array is full).
 */
U32 heap_insert (heap_t *h, S32 val);

/** Extract the greatest element, than run heapify_down over the values
 * array.
 *
 * @param h The heap;
 * @param val The position where the value will be positioned;
 * @return 0 on success, 1 on failure (array is empty).
 */
U32 heap_extract (heap_t *h, S32 *val);

/** Correct the heapify structure from below.
 *
 * This absumes that only the last element is not correctly stored.
 * Typically this happens when a new element has been added on the last
 * slot.
 *
 * @note Runs as assembly procedure, O(log n) on the number of stored
 *       elements.
 *
 * @param vals The values array;
 * @param used The total number of elements (last used slot included).
 */
void heapify_up (S32 vals[], size_t used);

/** Move the last element on top, and correct the heapify structure from
 * above.
 *
 * This absumes that the structure is already correctly stored.
 * The first value must be saved before this operation, since it gets
 * lost.
 *
 * This operation is logically equivalent to removing the greatest value
 * and rebuilding the heap structure. After running it you shuld decrement
 * of 1 the size of the used variable.
 *
 * @note Runs as assembly procedure, O(log n) on the number of stored
 *       elements.
 *
 * @param vals The values array;
 * @param used The total number of elements (last used slot included).
 */
void heapify_down (S32 vals[], size_t used);

/*@}*/

#endif // __NXOS_HEAP__

