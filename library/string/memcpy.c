/*
 * $Id: string_memcpy.c,v 1.8 2021-03-22 09:02:51 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

INLINE STATIC VOID
__memcpy(unsigned char *to, unsigned char *from, size_t len)
{
	/* The setup below is intended to speed up copying larger
	 * memory blocks. This can be very elaborate and should not be
	 * done unless a payoff can be expected.
	 */
	if (len > 4 * sizeof(long))
	{
		/* Try to align both source and destination to an even address. */
		if (IS_UNALIGNED(to) && IS_UNALIGNED(from))
		{
			(*to++) = (*from++);
			len--;
		}

		/* Try to align both source and destination to addresses which are
		 * multiples of four.
		 */
		if (len >= sizeof(short) && IS_SHORT_ALIGNED(to) && IS_SHORT_ALIGNED(from))
		{
			(*to++) = (*from++);
			(*to++) = (*from++);

			len -= sizeof(short);
		}

		/* If both source and destination are aligned to addresses which are
		 * multiples of four and there is still enough data left to be copied,
		 * try to move it in larger chunks.
		 */
		if (len >= sizeof(long) && IS_LONG_ALIGNED(to) && IS_LONG_ALIGNED(from))
		{
			unsigned long *_to = (unsigned long *)to;
			unsigned long *_from = (unsigned long *)from;

			/* An unrolled transfer loop, which shifts 32 bytes per iteration. */
			while (len >= 8 * sizeof(long))
			{
				/* The following should translate into load/store
				   opcodes which encode the access offsets (0..7)
				   into the respective displacement values. This
				   should help the PowerPC by avoiding pipeline
				   stalls (removing the postincrement on the address
				   will do that) but has no noticeable impact on the
				   68k platform (I checked). */

				_to[0] = _from[0];
				_to[1] = _from[1];
				_to[2] = _from[2];
				_to[3] = _from[3];
				_to[4] = _from[4];
				_to[5] = _from[5];
				_to[6] = _from[6];
				_to[7] = _from[7];

				_to += 8;
				_from += 8;

				/*
				(*_to++) = (*_from++);
				(*_to++) = (*_from++);
				(*_to++) = (*_from++);
				(*_to++) = (*_from++);
				(*_to++) = (*_from++);
				(*_to++) = (*_from++);
				(*_to++) = (*_from++);
				(*_to++) = (*_from++);
				*/

				len -= 8 * sizeof(long);
			}

			/* Try to mop up any small amounts of data still in need of
			 * copying...
			 */
			while (len >= sizeof(long))
			{
				(*_to++) = (*_from++);

				len -= sizeof(long);
			}

			to = (unsigned char *)_to;
			from = (unsigned char *)_from;
		}
	}

	/* If there's anything left, copy the rest. */
	while (len-- > 0)
		(*to++) = (*from++);
}

/****************************************************************************/

void *
memcpy(void *dst, const void *src, size_t len)
{
	void *result = dst;

	assert((len == 0) || (dst != NULL && src != NULL && (int)len > 0));

	if (dst == NULL || src == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	if (len > 0 && dst != src)
	{
		char *to = dst;
		const char *from = src;

		/* The two memory regions may not overlap. */
		assert((to) >= (from) + len || (from) >= (to) + len);

		/* Make sure __global_clib2 has been created */
		if (__global_clib2 != NULL) { 
			switch (__global_clib2->cpufamily) {
				case CPUFAMILY_4XX:
					__memcpy440((unsigned char *)to, (unsigned char *)from, len);
					break;
				default:
                    /* Check if we have altivec enabled */
                    if (__global_clib2->hasAltivec) {
                        _vec_memcpy((unsigned char *)to, (unsigned char *)from, len);
                    }
                    else {
                        /* Fallback to standard function */
                        __memcpy((unsigned char *)to, (unsigned char *)from, len);
                    }
			}
		}
		else {
			/* Fallback to standard function */
			__memcpy((unsigned char *)to, (unsigned char *)from, len);
		}
	}
	else
		__set_errno(EFAULT);

out:

	return (result);
}
