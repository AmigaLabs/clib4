/*
 * $Id: stdio_vasprintf.c,v 1.12 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#undef vasprintf

__static int
__vasprintf(const char *file, int line, char **ret, const char *format, va_list arg) {
    struct iob string_iob;
    int result = EOF;
    char local_buffer[32];

    ENTER();

    SHOWPOINTER(ret);
    SHOWSTRING(format);

    assert(ret != NULL && format != NULL && arg != NULL);

    if (ret == NULL || format == NULL || arg == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    (*ret) = NULL;

    __initialize_iob(&string_iob, __vasprintf_hook_entry,
                     NULL,
                     local_buffer, sizeof(local_buffer),
                     -1,
                     -1,
                     IOBF_IN_USE | IOBF_WRITE | IOBF_BUFFER_MODE_NONE | IOBF_INTERNAL,
                     NULL);

    string_iob.iob_String = NULL;
    string_iob.iob_StringSize = 0;
    string_iob.iob_File = (char *) file;
    string_iob.iob_Line = line;

    result = vfprintf((FILE * ) & string_iob, format, arg);
    if (result < 0) {
        SHOWMSG("ouch. that didn't work");

        if (string_iob.iob_String != NULL)
            free(string_iob.iob_String);

        goto out;
    }

    SHOWSTRING(string_iob.iob_String);

    (*ret) = string_iob.iob_String;

out:

    RETURN(result);
    return (result);
}

int
vasprintf(char **ret, const char *format, va_list arg) {
    int result;

    if (__check_abort_enabled)
        __check_abort();

    result = __vasprintf(NULL, 0, ret, format, arg);

    return (result);
}
