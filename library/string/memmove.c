/*
 * $Id: string_memmove.c,v 1.7 2006-09-22 09:02:51 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

INLINE STATIC VOID
__memmove(unsigned char *to, unsigned char *from, size_t len)
{
	if (from < to && to < from + len)
	{
		to += len;
		from += len;

		/* The setup below is intended to speed up copying larger
		 * memory blocks. This can be very elaborate and should not be
		 * done unless a payoff can be expected.
		 */
		if (len > 4 * sizeof(long))
		{
			size_t distance;

			/* Try to align both source and destination to an even address. */
			if (IS_UNALIGNED(to) && IS_UNALIGNED(from))
			{
				(*--to) = (*--from);
				len--;
			}

			/* Try to align both source and destination to addresses which are
			 * multiples of four.
			 */
			if (len >= sizeof(short) && IS_SHORT_ALIGNED(to) && IS_SHORT_ALIGNED(from))
			{
				(*--to) = (*--from);
				(*--to) = (*--from);

				len -= sizeof(short);
			}

			/* Check the distance between source and destination. If it's shorter
			 * than a long word, don't dive into the copying routine below since
			 * the overlapping copying may clobber data.
			 */
			distance = (size_t)(to - from);

			/* If both source and destination are aligned to addresses which are
			 * multiples of four and there is still enough data left to be copied,
			 * try to move it in larger chunks.
			 */
			if (distance >= sizeof(long) && len >= sizeof(long) && IS_LONG_ALIGNED(to) && IS_LONG_ALIGNED(from))
			{
				unsigned long *_to = (unsigned long *)to;
				unsigned long *_from = (unsigned long *)from;

				/* An unrolled transfer loop, which shifts 32 bytes per iteration. */
				while (len >= 8 * sizeof(long))
				{
					/* The following should translate into load/store
					   opcodes which encode the access offsets (-1..-8)
					   into the respective displacement values. This
					   should help the PowerPC by avoiding pipeline
					   stalls (removing the predecrement on the address
					   will do that) but has no noticeable impact on the
					   68k platform (I checked). */

					_to[-1] = _from[-1];
					_to[-2] = _from[-2];
					_to[-3] = _from[-3];
					_to[-4] = _from[-4];
					_to[-5] = _from[-5];
					_to[-6] = _from[-6];
					_to[-7] = _from[-7];
					_to[-8] = _from[-8];

					_to -= 8;
					_from -= 8;

					/*
					(*--_to) = (*--_from);
					(*--_to) = (*--_from);
					(*--_to) = (*--_from);
					(*--_to) = (*--_from);
					(*--_to) = (*--_from);
					(*--_to) = (*--_from);
					(*--_to) = (*--_from);
					(*--_to) = (*--_from);
					*/

					len -= 8 * sizeof(long);
				}

				/* Try to mop up any small amounts of data still in need of
				 * copying...
				 */
				while (len >= sizeof(long))
				{
					(*--_to) = (*--_from);

					len -= sizeof(long);
				}

				to = (unsigned char *)_to;
				from = (unsigned char *)_from;
			}
		}

		/* If there's anything left, copy the rest. */
		while (len-- > 0)
			(*--to) = (*--from);
	}
	else
	{
		/* The setup below is intended to speed up copying larger
		 * memory blocks. This can be very elaborate and should not be
		 * done unless a payoff can be expected.
		 */
		if (len > 4 * sizeof(long))
		{
			size_t distance;

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

			/* Check the distance between source and destination. If it's shorter
			 * than a long word, don't dive into the copying routine below since
			 * the overlapping copying may clobber data.
			 */
			if (to >= from)
				distance = (size_t)(to - from);
			else
				distance = (size_t)(from - to);

			/* If both source and destination are aligned to addresses which are
			 * multiples of four and there is still enough data left to be copied,
			 * try to move it in larger chunks.
			 */
			if (distance >= sizeof(long) && len >= sizeof(long) && IS_LONG_ALIGNED(to) && IS_LONG_ALIGNED(from))
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
}

/****************************************************************************/

void *
memmove(void *dest, const void *src, size_t len)
{
	void *result = dest;

	assert((len == 0) || (dest != NULL && src != NULL && (int)len > 0));

	if (dest == NULL || src == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	if (len > 0 && dest != src)
	{
		char *to = dest;
		const char *from = src;

		/* Make sure __global_clib2 has been created */
		if (__global_clib2 != NULL) { 
			switch (__global_clib2->cpufamily) {
				case CPUFAMILY_4XX:
					result = __memmove440((unsigned char *)to, (unsigned char *)from, len);
					break;
				default:
					__memmove((unsigned char *)to, (unsigned char *)from, len);
			}
		}
		else {
			/* Fallback to standard function */
			__memmove((unsigned char *)to, (unsigned char *)from, len);
		}
	}
	else
		__set_errno(EFAULT);

out:

	return (result);
}
