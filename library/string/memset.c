/*
 * $Id: string_memset.c,v 1.9 2021-03-22 09:02:51 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

void *
memset(void *ptr, int val, size_t len) {
	void *result = ptr;
	unsigned char *m = ptr;

    DECLARE_UTILITYBASE();

	assert((len == 0) || (ptr != NULL && (int)len > 0));

	if (ptr == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

    if (__global_clib2 != NULL && __global_clib2->optimizedCPUFunctions == TRUE) {
        /* Check if we have altivec enabled */
        if (__global_clib2->hasAltivec) {
            result = _vec_memset(m, (unsigned char)(val & 255), len);
        }
        else {
            /* Fallback to standard function */
            result = SetMem(ptr, val, len);
        }
    }
    else {
        /* Fallback to standard function */
        result = SetMem(ptr, val, len);
    }

out:

	return (result);
}
