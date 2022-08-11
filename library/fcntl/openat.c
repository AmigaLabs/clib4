/*
 * $Id: fcntl_openat.c,v 1.0 2022-08-10 12:04:22 clib2devs Exp $
*/
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

    if (filename == NULL)
    {
        __set_errno (EINVAL);
        return -1;
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
            return -1;
        }

        /* Check if fd is a directory */
        if (!S_ISDIR (st.st_mode)) {
            __set_errno(ENOTDIR);
            return -1;
        }

        /* Get curent dir */
        char *current_dir = get_current_dir_name();
        // Change dir to fd one
        chdir(st.st_name);
        // Do something with open in fd directory
        int file = open(filename, flags, mode);
        if (file > 0) {
            /* Restore old dir */
            chdir(current_dir);
        }
        return file;
    }
out:

    return -1;
}

