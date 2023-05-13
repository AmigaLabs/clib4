/*
 * $Id: string_memcmp.c,v 1.9 2023-04-04 15:33:36 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

inline static
int
__memcmp(const char *m1, const char *m2, size_t len) {
    int result = 0;

    assert(m1 != NULL && m2 != NULL && len > 0);

    /* The setup below is intended to speed up searching in larger
     * memory blocks. This can be very elaborate and should not be
     * done unless a payoff can be expected.
     */
    if (len > 4 * sizeof(long)) {
        /* Try to align both memory blocks to even addresses. */
        if (IS_UNALIGNED(m1) && IS_UNALIGNED(m2)) {
            len--;

            if ((*m1) != (*m2)) {
                result = (*m1) - (*m2);
                goto out;
            }

            m1++;
            m2++;
        }

        /* Try to align both memory blocks to addresses which are a
         * long word multiple.
         */
        if (len >= sizeof(short) && IS_SHORT_ALIGNED(m1) && IS_SHORT_ALIGNED(m2)) {
            const short *_m1 = (const short *) m1;
            const short *_m2 = (const short *) m2;

            /* Check if the two words are different, and if they are,
             * figure out down below which of the bytes were different.
             */
            if ((*_m1++) != (*_m2++))
                goto out;

            m1 = (const char *) _m1;
            m2 = (const char *) _m2;

            len -= sizeof(short);
        }

        /* If both memory blocks are aligned to addresses which can be
         * read directly one long word at a time, perform the comparison
         * in that manner.
         */
        if (len >= sizeof(long) && IS_LONG_ALIGNED(m1) && IS_LONG_ALIGNED(m2)) {
            const long *_m1 = (const long *) m1;
            const long *_m2 = (const long *) m2;

            do {
                /* If the two long words are different, figure out down
                 * below which of the bytes were different.
                 */
                if ((*_m1) != (*_m2)) {
                    m1 = (const char *) _m1;
                    m2 = (const char *) _m2;

                    goto out;
                }

                _m1++;
                _m2++;

                len -= sizeof(long);
            } while (len >= sizeof(long));

            m1 = (const char *) _m1;
            m2 = (const char *) _m2;
        }
    }

    out:

    /* If there are bytes left in need of comparison, take
     * care of them here. This also includes 'aborted'
     * comparison attempts from above.
     */
    while (len-- > 0) {
        if ((*m1) != (*m2)) {
            int b1, b2;

            /* The comparison must be performed as if the
               bytes were unsigned characters. */
            b1 = *(unsigned char *) m1;
            b2 = *(unsigned char *) m2;

            result = b1 - b2;
            break;
        }

        m1++;
        m2++;
    }

    return (result);
}

int
memcmp(const void *ptr1, const void *ptr2, size_t len) {
    int result = 0;
    struct _clib2 *__clib2 = __CLIB2;

    assert(ptr1 != NULL && ptr2 != NULL);
    assert((int) len >= 0);

    if (ptr1 == NULL || ptr2 == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    if (ptr1 != ptr2 && len > 0) {
        const char *m1 = ptr1;
        const char *m2 = ptr2;

        if (__clib2->__optimizedCPUFunctions) {
            switch (__clib2->cpufamily) {
                /* If we have a SAM4xx use specific version of function */
                case CPUFAMILY_4XX:
                    result = __memcmp440(m1, m2, len);
                    break;
#ifdef SPE
                case CPUFAMILY_E500:
                    result = __memcmp_e500(m1, m2, len);
                    break;
#endif
                default:
                    /* Check if we have altivec enabled */
                    if (__clib2->hasAltivec) {
                        result = vec_memcmp(m1, m2, len);
                    } else {
                        /* Fallback to standard function */
                        result = __memcmp(m1, m2, len);
                    }
            }
        } else {
            /* Fallback to standard function */
            result = __memcmp(m1, m2, len);
        }
    } else
        __set_errno(EFAULT);

    out:

    return (result);
}
