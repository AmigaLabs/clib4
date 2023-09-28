/*
 * $Id: stdio_vsprintf_hook_entry.c,v 1.5 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int64_t
__vsprintf_hook_entry(struct _clib4 *__clib4, struct iob *string_iob, struct file_action_message *fam) {
    int64_t result = ERROR;

    assert(fam != NULL && string_iob != NULL);

    if (fam->fam_Action != file_action_write) {
        fam->fam_Error = EBADF;
        goto out;
    }

    assert(fam->fam_Size >= 0);

    assert(fam->fam_Data != NULL);
    assert(string_iob->iob_StringPosition >= 0);

    memmove(&string_iob->iob_String[string_iob->iob_StringPosition], fam->fam_Data, (size_t) fam->fam_Size);
    string_iob->iob_StringPosition += fam->fam_Size;

    result = fam->fam_Size;

out:

    return (result);
}
