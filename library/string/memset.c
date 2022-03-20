/*
 * $Id: string_memset.c,v 1.9 2021-03-22 09:02:51 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

INLINE STATIC VOID
__memset(unsigned char *to, unsigned char value, size_t len)
{
	/* The setup below is intended to speed up changing larger
	 * memory blocks. This can be very elaborate and should not be
	 * done unless a payoff can be expected.
	 */
	if (len > 4 * sizeof(long))
	{
		if (IS_UNALIGNED(to))
		{
			(*to++) = value;
			len--;
		}

		if (len >= sizeof(short) && IS_SHORT_ALIGNED(to))
		{
			(*to++) = value;
			(*to++) = value;

			len -= sizeof(short);
		}

		if (len >= sizeof(long) && IS_LONG_ALIGNED(to))
		{
			unsigned long *_to = (unsigned long *)to;
			unsigned long _value = value;

			_value |= (_value << 8);
			_value |= (_value << 16);

			while (len >= 8 * sizeof(long))
			{
				/* The following should translate into load/store
				   opcodes which encode the access offsets (0..7)
				   into the respective displacement values. This
				   should help the PowerPC by avoiding pipeline
				   stalls (removing the postincrement on the address
				   will do that) but has no noticeable impact on the
				   68k platform (I checked). */

				_to[0] = _value;
				_to[1] = _value;
				_to[2] = _value;
				_to[3] = _value;
				_to[4] = _value;
				_to[5] = _value;
				_to[6] = _value;
				_to[7] = _value;

				_to += 8;

				len -= 8 * sizeof(long);
			}

			while (len >= sizeof(long))
			{
				(*_to++) = _value;

				len -= sizeof(long);
			}

			to = (unsigned char *)_to;
		}
	}

	while (len-- > 0)
		(*to++) = value;
}

void *
memset(void *ptr, int val, size_t len)
{
	void *result = ptr;
	unsigned char *m = ptr;

	assert((len == 0) || (ptr != NULL && (int)len > 0));

	if (ptr == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	__memset(m, (unsigned char)(val & 255), len);

out:

	return (result);
}
