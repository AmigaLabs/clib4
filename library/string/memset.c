/*
 * $Id: string_memset.c,v 1.10 2023-02-22 09:02:51 clib4devs Exp $
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

    if (__IUtility == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

	if (ptr == NULL) {
		__set_errno(EFAULT);
		goto out;
	}

    result = SetMem(ptr, val, len);

out:

	return result;
}
