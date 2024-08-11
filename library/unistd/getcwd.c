/*
 * $Id: unistd_getcwd.c,v 1.10 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

char *
getcwd(char *buffer, size_t buffer_size) {
    struct name_translation_info buffer_nti;
    char *result = NULL;
    BPTR dir_lock = BZERO;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(buffer);
    SHOWVALUE(buffer_size);

	__check_abort_f(__clib4);

    if (buffer_size == 0 || buffer == NULL) {
        /* As an extension to the POSIX.1-2001 standard, glibc's getcwd()
         * allocates the buffer dynamically using malloc(3) if buf is NULL.
         * In this case, the allocated buffer has the length size unless
         * size is zero, when buf is allocated as big as necessary.  The
         * caller should free(3) the returned buffer.
        */
        buffer = __malloc_r(__clib4, PATH_MAX);
        buffer_size = PATH_MAX;
        if (buffer == NULL) {
            SHOWMSG("not enough memory for result buffer");

            __set_errno(ENOMEM);
            goto out;
        }
    }

    dir_lock = Lock("", SHARED_LOCK);
    if (dir_lock == BZERO) {
        SHOWMSG("could not get a lock on the current directory");

        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    if (__clib4->__unix_path_semantics) {
        if (__clib4->__current_path_name[0] != '\0') {
            if (buffer_size < strlen(__clib4->__current_path_name) + 1) {
                SHOWMSG("buffer is too small");

                __set_errno(ERANGE);
                goto out;
            }

            strcpy(buffer, __clib4->__current_path_name);

            D(("returning absolute path name '%s'", buffer));

            result = buffer;
        }
    }

    if (result == NULL) {
        LONG status;

        status = NameFromLock(dir_lock, buffer, (LONG) buffer_size);
        if (status == DOSFALSE) {
            int errno_code;
            LONG io_error;

            SHOWMSG("could not get name from lock");

            io_error = IoErr();

            /* Was the buffer too small? */
            if (io_error == ERROR_LINE_TOO_LONG)
                errno_code = ERANGE;
            else
                errno_code = __translate_io_error_to_errno(io_error);

            __set_errno(errno_code);
            goto out;
        }

        if (__clib4->__unix_path_semantics) {
            const char *path_name = buffer;

            if (__translate_amiga_to_unix_path_name(&path_name, &buffer_nti) != 0)
                goto out;

            if (buffer_size < strlen(path_name) + 1) {
                SHOWMSG("buffer is too small");

                __set_errno(ERANGE);
                goto out;
            }

            strcpy(buffer, path_name);
        }
    }

    SHOWSTRING(buffer);

    result = buffer;

out:

    UnLock(dir_lock);

    RETURN(result);
    return (result);
}
