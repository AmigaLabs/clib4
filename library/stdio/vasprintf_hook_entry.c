/*
 * $Id: stdio_vasprintf_hook_entry.c,v 1.8 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

// TODO - Check for 64bits malloc here

int64_t
__vasprintf_hook_entry(
        struct _clib4 *__clib4,
        struct iob *string_iob,
        struct file_action_message *fam) {
    int64_t result = EOF;
    int64_t num_bytes_left;
    int64_t num_bytes;

    assert(fam != NULL && string_iob != NULL);
    assert(fam->fam_Action == file_action_write);

    if (fam->fam_Action != file_action_write) {
        fam->fam_Error = EBADF;
        goto out;
    }

    if (string_iob->iob_StringPosition + fam->fam_Size > string_iob->iob_StringSize) {
        const int granularity = 64;

        size_t new_size;
        char *buffer;

        new_size = string_iob->iob_StringPosition + fam->fam_Size + granularity;

        buffer = __malloc_r(__clib4, new_size);
        if (buffer == NULL) {
            fam->fam_Error = ENOBUFS;
            goto out;
        }

        if (string_iob->iob_String != NULL) {
            memmove(buffer, string_iob->iob_String, (size_t) string_iob->iob_StringSize);

            __free_r(__clib4, string_iob->iob_String);
        }

        string_iob->iob_String = buffer;
        string_iob->iob_StringSize = new_size;
    }

    assert(string_iob->iob_StringPosition <= string_iob->iob_StringSize);

    num_bytes_left = string_iob->iob_StringSize - string_iob->iob_StringPosition;

    num_bytes = fam->fam_Size;
    if (num_bytes > num_bytes_left)
        num_bytes = num_bytes_left;

    assert(num_bytes >= 0);
    assert(fam->fam_Data != NULL);

    memmove(&string_iob->iob_String[string_iob->iob_StringPosition], fam->fam_Data, (size_t) num_bytes);
    string_iob->iob_StringPosition += num_bytes;

    result = num_bytes;

out:

    return (result);
}
