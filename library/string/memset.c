/*
 * $Id: string_memset.c,v 1.10 2023-02-22 09:02:51 clib2devs Exp $
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

    ENTER();
    DECLARE_UTILITYBASE();
    SHOWPOINTER(__UtilityBase);
    SHOWPOINTER(__IUtility);

	assert((len == 0) || (ptr != NULL && (int)len > 0));

	if (ptr == NULL) {
		__set_errno(EFAULT);
		goto out;
	}

    result = SetMem(ptr, val, len);

out:
    RETURN(result);
	return (result);
}
