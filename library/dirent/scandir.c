/*
 * $Id: dirent_scandir.c,v 1.0 2022-08-17 12:04:22 clib4devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

int scandir(const char *path, struct dirent ***res,
            int (*sel)(const struct dirent *),
            int (*cmp)(const struct dirent **, const struct dirent **)) {
    DIR *d = opendir(path);
    struct dirent *de, **names = 0, **tmp;
    size_t cnt = 0, len = 0;
    int old_errno = errno;

    if (!d) {
        __set_errno(EACCES);
        return -1;
    }

    while ((errno = 0), (de = readdir(d))) {
        if (sel && !sel(de))
            continue;
        if (cnt >= len) {
            len = 2 * len + 1;
            if (len > SIZE_MAX / sizeof *names)
                break;
            tmp = realloc(names, len * sizeof *names);
            if (!tmp)
                break;
            names = tmp;
        }
        names[cnt] = malloc(de->d_reclen);
        if (!names[cnt])
            break;
        memcpy(names[cnt++], de, de->d_reclen);
    }

    closedir(d);

    if (errno) {
        if (names)
            while (cnt-- > 0) free(names[cnt]);
        free(names);
        return -1;
    }
    __set_errno(old_errno);

    if (cmp)
        qsort(names, cnt, sizeof *names, (int (*)(const void *, const void *)) cmp);
    *res = names;
    return cnt;
}