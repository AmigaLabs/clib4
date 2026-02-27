/*
 * $Id: stdio_translateioerror.c,v 1.4 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

int
__translate_io_error_to_errno(LONG io_err) {
    static const struct {
        LONG io_err;
        LONG _errno;
    } map_table[] =
        {
            {ERROR_NO_FREE_STORE,          ENOMEM},
            {ERROR_TASK_TABLE_FULL,        ENOMEM},
            {ERROR_BAD_TEMPLATE,           EINVAL},
            {ERROR_BAD_NUMBER,             EINVAL},
            {ERROR_REQUIRED_ARG_MISSING,   EINVAL},
            {ERROR_KEY_NEEDS_ARG,          EINVAL},
            {ERROR_TOO_MANY_ARGS,          EINVAL},
            {ERROR_UNMATCHED_QUOTES,       EINVAL},
            {ERROR_LINE_TOO_LONG,          ENAMETOOLONG},
            {ERROR_FILE_NOT_OBJECT,        ENOEXEC},
            {ERROR_OBJECT_IN_USE,          EBUSY},
            {ERROR_OBJECT_EXISTS,          EEXIST},
            {ERROR_DIR_NOT_FOUND,          ENOENT},
            {ERROR_OBJECT_NOT_FOUND,       ENOENT},
            {ERROR_BAD_STREAM_NAME,        EINVAL},
            {ERROR_OBJECT_TOO_LARGE,       EFBIG},
            {ERROR_ACTION_NOT_KNOWN,       ENOSYS},
            {ERROR_INVALID_COMPONENT_NAME, EINVAL},
            {ERROR_INVALID_LOCK,           EBADF},
            {ERROR_OBJECT_WRONG_TYPE,      EFTYPE},
            {ERROR_DISK_NOT_VALIDATED,     EROFS},
            {ERROR_DISK_WRITE_PROTECTED,   EROFS},
            {ERROR_RENAME_ACROSS_DEVICES,  EXDEV},
            {ERROR_DIRECTORY_NOT_EMPTY,    ENOTEMPTY},
            {ERROR_TOO_MANY_LEVELS,        ENAMETOOLONG},
            {ERROR_DEVICE_NOT_MOUNTED,     ENXIO},
            {ERROR_COMMENT_TOO_BIG,        ENAMETOOLONG},
            {ERROR_DISK_FULL,              ENOSPC},
            {ERROR_DELETE_PROTECTED,       EACCES},
            {ERROR_WRITE_PROTECTED,        EACCES},
            {ERROR_READ_PROTECTED,         EACCES},
            {ERROR_NOT_A_DOS_DISK,         EFTYPE},
            {ERROR_NO_DISK,                EACCES},
            {ERROR_IS_SOFT_LINK,           EFTYPE},
            {ERROR_BAD_HUNK,               ENOEXEC},
            {ERROR_NOT_IMPLEMENTED,        ENOSYS},
            {ERROR_LOCK_COLLISION,         EACCES},
            {ERROR_BREAK,                  EINTR},
            {ERROR_NOT_EXECUTABLE,         ENOEXEC},
            {ERROR_WOULD_BLOCK,            EWOULDBLOCK}
        };

    unsigned int i;
    int result;

    result = EIO;

    for (i = 0; i < NUM_ENTRIES(map_table); i++) {
        if (map_table[i].io_err == io_err) {
            result = map_table[i]._errno;
            break;
        }
    }

    return (result);
}

/****************************************************************************/

/* Same as above, except that we translate ERROR_OBJECT_WRONG_TYPE
   into ENOTDIR by default. */
int
__translate_access_io_error_to_errno(LONG io_err) {
    int result;

    if (io_err == ERROR_OBJECT_WRONG_TYPE)
        result = ENOTDIR;
    else
        result = __translate_io_error_to_errno(io_err);

    return (result);
}
