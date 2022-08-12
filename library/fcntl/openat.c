/*
 * $Id: fcntl_openat.c,v 1.0 2022-08-10 12:04:22 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

int
openat(int fd, const char *filename, int flags, ...) {
    mode_t mode = 0;
    struct name_translation_info path_name_nti;
    BOOL absolute = FALSE;
    int result = -1;

    if (filename == NULL)
    {
        __set_errno (EINVAL);
        goto out;
    }

    /* Check for relative path */
    if (__unix_path_semantics) {
        if (filename[0] == '/')
            absolute = TRUE;

        if (__translate_unix_to_amiga_path_name(&filename, &path_name_nti) != 0)
            goto out;
    }
    else {
        if (strchr(filename, ':') != NULL)
            absolute = TRUE;
    }

    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, mode_t);
        va_end(ap);
    }
    if (absolute || fd == AT_FDCWD)
        return open(filename, flags, mode);
    else {
        struct stat st;
        if (fstat(fd, &st) < 0) {
            goto out;
        }

        /* Check if fd is a directory */
        if (!S_ISDIR (st.st_mode)) {
            __set_errno(ENOTDIR);
            goto out;
        }

        struct fd *fd1 = __get_file_descriptor(fd);
        if (fd1 == NULL) {
            __set_errno(EBADF);
            goto out;
        }

        __fd_lock(fd1);

        /* Get curent dir */
        char *current_dir = get_current_dir_name();
        // Change dir to fd one
        if (chdir(fd1->fd_Aux) == OK) {
            // Do something with open in fd directory
            result = open(filename, flags, mode);

            /* Restore old dir */
            chdir(current_dir);
        }
        else {
            __fd_unlock(fd1);

            __set_errno(ENOTDIR);
            goto out;
        }

        __fd_unlock(fd1);
    }
out:

    return result;
}

