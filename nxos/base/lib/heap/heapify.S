/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

    .text

/* int get_root (int) */
get_root:
    mov     r1, #1
    tst     r0, #1          /* Check if r0 is even */
    subne   r0, r0, #1      /* Subtract 1 if r0 is odd */
    mov     r0, r0, lsr #1  /* Divide by 2 (shift of 1) in both cases */
    subeq   r0, r0, #1      /* Subtract 1 if r0 is even */
    bx      lr

/* int get_right (int) */
get_right:
    add     r0, r0, #1      /* Add 1 */
    mov     r0, r0, lsl #1  /* Multiply by 2 */
    bx      lr

/* int get_left (int) */
get_left:
    mov     r0, r0, lsl #1  /* Multiply by 2 */
    add     r0, r0, #1      /* Add 1 */
    bx      lr
    
    .global heapify_up
heapify_up:
    mov     ip, sp
    stmfd   sp!, {r4, fp, ip, lr, pc}

    mov     r12, r0         /* r12 points to the values array */
    subs    r4, r1, #1      /* r1 contains the cursor index, must be moved
                               to r4 (avoid modification by get_root) */

    /* Return if there are no elements (r4 = 0 or r4 = -1) */
    ldmlefd sp, {r4, fp, sp, pc}

    mov     r0, r4          /* Argument for get_root */

hup_loop:
    /* r1 is used here inside */
    bl      get_root

    /* r0 = index of root       -> r2 value of root
     * r4 = index of cursor     -> r3 value of cursor
     */
    ldr     r2, [r12, r0, lsl#2]
    ldr     r3, [r12, r4, lsl#2]

    cmp     r2, r3
    /* Otherwise we store them swapped and repeat */
    strle   r2, [r12, r4, lsl#2]
    strle   r3, [r12, r0, lsl#2]

    /* If values[cursor] <= values[root] we can exit */
    ldmgtfd   sp, {r4, fp, sp, pc}

    mov     r4, r0
    b       hup_loop

    .global heapify_down
heapify_down:
    mov     ip, sp
    stmfd   sp!, {r4, r5, r6, fp, ip, lr, pc}

    mov     r12, r0         /* r12 points to the values array */    
    subs    r4, r1, #1      /* r4 index of the last position */

    /* Return if there are no elements (r4 = 0 or r4 = -1) */
    ldmeqfd sp, {r4, r5, r6, fp, sp, pc}

    /* Move the last element in the first position */
    ldr     r3, [r12, r4, lsl#2]
    str     r3, [r12]

    // r12 is keeping the array
    // r4 is the boundary
    // r0, r1 will be used by left and right functions
    // r2, r3 will be used respectively for left index and right index
    // r5 is always keeping the current cursor

    mov     r5, #0      /* We start from 0 */
hdown_loop:
    /* Retrieving left */
    mov     r0, r5
    bl      get_left
    cmp     r0, r4      /* We'll exit if left is greater than
                         * the boundary */
    ldmgefd sp, {r4, r5, r6, fp, sp, pc}
    mov     r2, r0

    /* Retrieving right */
    mov     r0, r5
    bl      get_right
    movlt   r3, r0

    /* Now r0 and r1 can be reused */

    /* r6 will contain the index of the position to be swapped if any.
     * Otherwise we simply exit, while r0 will contain the value of the
     * position to be swapped. */

    cmp     r3, r4      /* Compare right and boundary */
    mov     r6, r2      /* For the moment we take left */

    /* Load the left value in r0 (must be loaded in any case) */
    ldr     r0, [r12, r2, lsl#2]

    bgt     hdown_bfound    /* We don't have any right branch. Branch
                               found! */

    /* Load also right values into r1 respectively, r1 only if
     * needed */
    ldr     r1, [r12, r3, lsl#2]

    cmp     r0, r1          /* Comparsion between left and right values */
    movlt   r6, r3          /* Right is greater than left, we load */
    movlt   r0, r1          /*       right's index and value */

hdown_bfound:

    // remember to update r5 with the next cursor
    // do not modify r4
    // you can use r1, r2 and r3

    /* Now we load the cursor's position in r1 and we compare it with the
     * greatest of left and right */
    ldr     r1, [r12, r5, lsl#2]
    cmp     r1, r0

    str     r0, [r12, r5, lsl#2]    /* Store the new root value */
    str     r1, [r12, r6, lsl#2]    /* Store the new cursor value */
    mov     r5, r6                  /* Update new cursor */
    blt     hdown_loop

    ldmfd   sp, {r4, r5, r6, fp, sp, pc}  /* Heapified! Now exit */


