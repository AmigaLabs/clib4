/*
 * $Id: ftw_ftw.c,v 1.5 2006-01-08 12:04:22 clib4devs Exp $
*/

/*
 * This ftw() implementation builds heavily on existing functionality and could certainly
 * be made more efficient (e.g. by combining  walk & stat using ExAll()/ExNext()) but this
 * way the Unix pathname translation is free.
 */

#ifndef    _FTW_HEADERS_H
#include "ftw_headers.h"
#endif /* _FTW_HEADERS_H */

static int
walk(const char *path, int (*func)(const char *, const struct stat *, int), int depth, int level) {
    int result = OK;
    DIR *dp;
    struct dirent *de;
    struct stat st;
    char *next_name;
    int type;
    int old_length;
    int errtmp;

    ENTER();

    SHOWSTRING(path);
    SHOWPOINTER(func);
    SHOWVALUE(depth);
    SHOWVALUE(level);

    __check_abort();

    if (level > depth)
        goto out;

    if (stat(path, &st) == OK) {
        if (S_ISDIR(st.st_mode)) {
            if (FLAG_IS_SET(st.st_mode, S_IRUSR))
                type = FTW_D;    /* Directory */
            else
                type = FTW_DNR;    /* Directory No Read-permission */
        } else if (S_ISREG(st.st_mode)) {
            type = FTW_F; /* File */
        } else {
            /* Skip */
            goto out;
        }
    } else {
        type = FTW_NS; /* No Stat */
    }

    result = (*func)(path, &st, type);
    if (result != 0)
        goto out;

    if (type == FTW_D) {
        dp = opendir(path);    /* Also takes care of Unix->Amiga pathname conversion. */
        if (dp == NULL) {
            result = ERROR; /* Pass errno from opendir() */
            goto out;
        }

        old_length = strlen(path);

        next_name = malloc(old_length + NAME_MAX + 2); /* Allocate new for each recursive step to handle extremely long path names. */
        if (next_name == NULL) {
            __set_errno(ENOMEM);

            result = ERROR;
            goto out;
        }

        strcpy(next_name, path);

        if (old_length > 0 && next_name[old_length - 1] != ':' && next_name[old_length - 1] != '/')
            next_name[old_length++] = '/';

        while ((result == 0) && (de = readdir(dp)) != NULL) {
            strlcpy(&next_name[old_length], de->d_name, NAME_MAX + 2);

            result = walk(next_name, func, depth, level + 1);
        }

        errtmp = __get_errno();

        closedir(dp);
        free(next_name);

        __set_errno(errtmp);
    }

out:

    RETURN(result);
    return (result);
}

int
ftw(const char *path, int (*func)(const char *, const struct stat *, int), int depth) {
    int result = ERROR;
    char *base;
    int len;

    ENTER();

    SHOWSTRING(path);
    SHOWPOINTER(func);
    SHOWVALUE(depth);

    if (path == NULL) {
        SHOWMSG("NULL pathname to ftw().");

        __set_errno(EFAULT);
        goto out;
    }

    if (func == NULL) {
        SHOWMSG("No function supplied.");

        __set_errno(EFAULT);
        goto out;
    }

    if (depth < 0) {
        SHOWMSG("Invalid depth.\n");

        __set_errno(EINVAL);
        goto out;
    }

    /* Make sure the path never ends with '/' unless used to indicate parent directory. */
    len = strlen(path);
    if (len > 1 && path[len - 1] == '/' && path[len - 2] != '/') {
        int error;

        base = strdup(path);
        if (base == NULL) {
            __set_errno(ENOMEM);
            goto out;
        }

        base[len - 1] = '\0';

        result = walk(base, func, depth, 0);

        error = __get_errno();

        free(base);

        __set_errno(error);
    } else {
        result = walk(path, func, depth, 0);
    }

out:

    RETURN(result);
    return (result);
}
