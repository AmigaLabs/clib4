/*
 * $Id: stdio_tmpfile.c,v 1.6 2024-07-22 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

FILE *
tmpfile(void) {
    BPTR temp_file_lock = BZERO;
    char *temp_file_name;
    FILE *result = NULL;
    struct iob *file;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    __check_abort_f(__clib4);

    temp_file_name = __malloc_r(__clib4, L_tmpnam);
    if (temp_file_name == NULL) {
        SHOWMSG("not enough memory for temp file name");
        goto out;
    }

    /* Get a temp_file_lock on the current directory; it will be needed
     * later when the temporary file is deleted.
     */

    temp_file_lock = Lock("", SHARED_LOCK);
    if (temp_file_lock == BZERO) {
        SHOWMSG("couldn't get a temp_file_lock on the current directory");

        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    /* Find a new temporary file name. */
    tmpnam(temp_file_name);

    SHOWSTRING(temp_file_name);

    /* Open that file; we're going to need it in a minute. */
    file = (struct iob *) fopen(temp_file_name, "w+");
    if (file == NULL) {
        SHOWMSG("file didn't open");
        goto out;
    }

    /* Remember this for later when the temporary file is
     * to be deleted.
     */
    SET_FLAG(file->iob_Flags, IOBF_TEMP);

    file->iob_TempFileName = temp_file_name;
    temp_file_name = NULL;

    file->iob_TempFileLock = temp_file_lock;
    temp_file_lock = BZERO;

    result = (FILE *) file;

out:

    UnLock(temp_file_lock);

    if (temp_file_name != NULL)
        __free_r(__clib4, temp_file_name);

    RETURN(result);
    return (result);
}
