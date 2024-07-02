/*
 * $Id: stdlib_qsort.c,v 1.6 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

typedef int (*__compar_d_fn_t) (__const void *, __const void *, void *);

/* Byte-wise swap two items of size SIZE. */
#define SWAP(a, b, size)                              \
  do                                          \
    {                                          \
      register size_t __size = (size);                          \
      register char *__a = (a), *__b = (b);                      \
      do                                      \
    {                                      \
      char __tmp = *__a;                              \
      *__a++ = *__b;                              \
      *__b++ = __tmp;                              \
    } while (--__size > 0);                              \
    } while (0)

/* Discontinue quicksort algorithm when partition gets below this size.
   This particular magic number was chosen to work best on a Sun 4/260. */
#define MAX_THRESH 4

/* Stack node declarations used to store unfulfilled partition obligations. */
typedef struct {
    char *lo;
    char *hi;
} stack_node;


/* The next 4 #defines implement a very fast in-line stack abstraction. */
/* The stack needs log (total_elements) entries (we could even subtract
   log(MAX_THRESH)).  Since total_elements has type size_t, we get as
   upper bound for log (total_elements):
   bits per byte (CHAR_BIT) * sizeof(size_t).  */
#define Q_STACK_SIZE    (CHAR_BIT * sizeof(size_t))
#define Q_PUSH(low, high)    ((void) ((top->lo = (low)), (top->hi = (high)), ++top))
#define Q_POP(low, high)    ((void) (--top, (low = top->lo), (high = top->hi)))
#define Q_STACK_NOT_EMPTY    (stack < top)

void
qsort(void *pbase, size_t total_elems, size_t size, int (*compare)(const void *, const void *)) {
    register char *base_ptr = (char *) pbase;

    const size_t max_thresh = MAX_THRESH * size;

    if (total_elems == 0)
        /* Avoid lossage with unsigned arithmetic below.  */
        return;

    if (total_elems > MAX_THRESH) {
        char *lo = base_ptr;
        char *hi = &lo[size * (total_elems - 1)];
        stack_node stack[Q_STACK_SIZE];
        stack_node *top = stack;

        Q_PUSH(NULL, NULL);

        while (Q_STACK_NOT_EMPTY) {
            char *left_ptr;
            char *right_ptr;

            /* Select median value from among LO, MID, and HI. Rearrange
               LO and HI so the three values are sorted. This lowers the
               probability of picking a pathological pivot value and
               skips a comparison for both the LEFT_PTR and RIGHT_PTR in
               the while loops. */

            char *mid = lo + size * ((hi - lo) / size >> 1);

            if ((*compare)((void *) mid, (void *) lo) < 0)
                SWAP(mid, lo, size);
            if ((*compare)((void *) hi, (void *) mid) < 0)
                SWAP(mid, hi, size);
            else
                goto jump_over;
            if ((*compare)((void *) mid, (void *) lo) < 0)
                SWAP(mid, lo, size);
            jump_over:;

            left_ptr = lo + size;
            right_ptr = hi - size;

            /* Here's the famous ``collapse the walls'' section of quicksort.
               Gotta like those tight inner loops!  They are the main reason
               that this algorithm runs much faster than others. */
            do {
                while ((*compare)((void *) left_ptr, (void *) mid) < 0)
                    left_ptr += size;

                while ((*compare)((void *) mid, (void *) right_ptr) < 0)
                    right_ptr -= size;

                if (left_ptr < right_ptr) {
                    SWAP(left_ptr, right_ptr, size);
                    if (mid == left_ptr)
                        mid = right_ptr;
                    else if (mid == right_ptr)
                        mid = left_ptr;
                    left_ptr += size;
                    right_ptr -= size;
                } else if (left_ptr == right_ptr) {
                    left_ptr += size;
                    right_ptr -= size;
                    break;
                }
            } while (left_ptr <= right_ptr);

            /* Set up pointers for next iteration.  First determine whether
               left and right partitions are below the threshold size.  If so,
               ignore one or both.  Otherwise, push the larger partition's
               bounds on the stack and continue sorting the smaller one. */

            if ((size_t)(right_ptr - lo) <= max_thresh) {
                if ((size_t)(hi - left_ptr) <= max_thresh)
                    /* Ignore both small partitions. */
                    Q_POP (lo, hi);
                else
                    /* Ignore small left partition. */
                    lo = left_ptr;
            } else if ((size_t)(hi - left_ptr) <= max_thresh)
                /* Ignore small right partition. */
                hi = right_ptr;
            else if ((right_ptr - lo) > (hi - left_ptr)) {
                /* Push larger left partition indices. */
                Q_PUSH(lo, right_ptr);
                lo = left_ptr;
            } else {
                /* Push larger right partition indices. */
                Q_PUSH(left_ptr, hi);
                hi = right_ptr;
            }
        }
    }

    /* Once the BASE_PTR array is partially sorted by quicksort the rest
       is completely sorted using insertion sort, since this is efficient
       for partitions below MAX_THRESH size. BASE_PTR points to the beginning
       of the array to sort, and END_PTR points at the very last element in
       the array (*not* one beyond it!). */


    {
        char *const end_ptr = &base_ptr[size * (total_elems - 1)];
        char *tmp_ptr = base_ptr;
        char *thresh = min(end_ptr, base_ptr + max_thresh);
        register char *run_ptr;

        /* Find smallest element in first threshold and place it at the
           array's beginning.  This is the smallest array element,
           and the operation speeds up insertion sort's inner loop. */

        for (run_ptr = tmp_ptr + size; run_ptr <= thresh; run_ptr += size)
            if ((*compare)((void *) run_ptr, (void *) tmp_ptr) < 0)
                tmp_ptr = run_ptr;

        if (tmp_ptr != base_ptr)
            SWAP(tmp_ptr, base_ptr, size);

        /* Insertion sort, running from left-hand-side up to right-hand-side.  */

        run_ptr = base_ptr + size;
        while ((run_ptr += size) <= end_ptr) {
            tmp_ptr = run_ptr - size;
            while ((*compare)((void *) run_ptr, (void *) tmp_ptr) < 0)
                tmp_ptr -= size;

            tmp_ptr += size;
            if (tmp_ptr != run_ptr) {
                char *trav;

                trav = run_ptr + size;
                while (--trav >= run_ptr) {
                    char c = *trav;
                    char *hi, *lo;

                    for (hi = lo = trav; (lo -= size) >= tmp_ptr; hi = lo)
                        *hi = *lo;
                    *hi = c;
                }
            }
        }
    }
}
