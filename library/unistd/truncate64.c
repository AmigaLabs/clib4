/*
 * $Id: unistd_truncate.c,v 1.0 2021-02-05 18:16:39 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
truncate64(const char *path_name, _off64_t length)
{
    int result = ERROR;
    int fd;

    ENTER();

    SHOWSTRING(path_name);
    SHOWVALUE(length);

    assert(path_name != NULL);

    if (__check_abort_enabled)
        __check_abort();

    if (path_name == NULL)
    {
        SHOWMSG("invalid path name");

        __set_errno(EFAULT);
        goto out;
    }

    if (length < 0)
    {
        SHOWMSG("invalid length");

        __set_errno(EINVAL);
        goto out;
    }

    fd = open(path_name, O_WRONLY);
    if (fd < 0)
    {
        SHOWMSG("file didn't open");
        goto out;
    }

    result = ftruncate64(fd, length);

    close(fd);

out:

    RETURN(result);
    return (result);
}
