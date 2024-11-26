/*
 * $Id: fcntl_openat.c,v 1.0 2022-08-10 12:04:22 clib4devs Exp $
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
    struct _clib4 *__clib4 = __CLIB4;

    if (filename == NULL) {
        __set_errno (EINVAL);
        goto out;
    }

    /* Check for relative path */
    if (__clib4->__unix_path_semantics) {
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

        /* Get the file descriptor */
        struct fd *fd1 = __get_file_descriptor(__clib4, fd);
        if (fd1 == NULL) {
            __set_errno(EBADF);
            goto out;
        }

        /* Create file name */
        char filename_complete[MAXPATHLEN + 1] = {0};

        __fd_lock(fd1);

        char *fn = (char *) fd1->fd_Aux;
        /* Create the filename to open based on directory and filename
         * Check if we are accessing a directory that ends with :
         * If so don't add a / in the file name
        */
        if (fn[strlen(fn) - 1] == ':')
            snprintf(filename_complete, MAXPATHLEN, "%s%s", fn, filename);
        else
            snprintf(filename_complete, MAXPATHLEN, "%s/%s", fn, filename);

        __fd_unlock(fd1);

        // Do something with open in fd directory
        result = open(filename_complete, flags, mode);
    }
out:

    return result;
}

