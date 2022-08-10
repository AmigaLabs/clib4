/*
 * $Id: unistd_getwd.c,v 1.0 2022-05-06 09:35:04 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

char *
getwd(char *buf) {
    ENTER();

    SHOWPOINTER(buf);

    assert(buf != NULL);

    __check_abort();

    char tmpbuf[PATH_MAX];

    if (buf == NULL) {
        __set_errno(EINVAL);
        goto out;
    }

    if (strlen(buf) + 1 > PATH_MAX) {
        SHOWMSG("path is too long");

        __set_errno(ENAMETOOLONG);
        goto out;
    }

    if (getcwd(tmpbuf, PATH_MAX) == NULL) {
        /* We use 1024 here since it should really be enough and because
       this is a safe value.  */
        strerror_r(errno, buf, 1024);
        goto out;
    }

    /* This is completely unsafe.  Nobody can say how big the user
       provided buffer is.  Perhaps the application and the libc
       disagree about the value of PATH_MAX.  */
    RETURN(strcpy(buf, tmpbuf));
    return strcpy(buf, tmpbuf);

out:
    RETURN(NULL);
    return (NULL);
}