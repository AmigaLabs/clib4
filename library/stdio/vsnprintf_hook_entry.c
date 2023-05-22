/*
 * $Id: stdio_vsnprintf_hook_entry.c,v 1.8 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

// TODO - Check for memmove 64bits

int64_t
__vsnprintf_hook_entry(struct _clib2 *__clib2, struct iob *string_iob, struct file_action_message *fam) {
    int64_t result = EOF;

    assert(fam != NULL && string_iob != NULL);

    if (fam->fam_Action != file_action_write) {
        fam->fam_Error = EBADF;
        goto out;
    }

    if (fam->fam_Size > 0 && string_iob->iob_StringSize > 0 &&
        string_iob->iob_StringPosition < string_iob->iob_StringSize) {
        int64_t num_bytes_left;
        int64_t num_bytes;

        num_bytes_left = string_iob->iob_StringSize - string_iob->iob_StringPosition;

        num_bytes = fam->fam_Size;
        if (num_bytes > num_bytes_left)
            num_bytes = num_bytes_left;

        assert(num_bytes >= 0);

        assert(fam->fam_Data != NULL);
        assert(string_iob->iob_String != NULL);

        memmove(&string_iob->iob_String[string_iob->iob_StringPosition], fam->fam_Data, (size_t) num_bytes);
        string_iob->iob_StringPosition += num_bytes;
    }

    result = fam->fam_Size;

out:

    return (result);
}
