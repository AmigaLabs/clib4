/*
 * $Id: ipc_ftok.c,v 1.0 2021-01-26 12:19:14 clib2devs Exp $
*/

/****************************************************************************/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <sys/ipc.h>

key_t ftok(const char *path, int id)
{
    key_t key = (key_t)-1;
    BPTR lock = 0;
    uint32_t blockno = 0;
    struct Lock *flock;

    if (path)
    {

        lock = Lock(path, SHARED_LOCK);

        if (lock)
        {
            flock = BADDR(lock);
            blockno = flock->fl_Key;
            UnLock(lock);
            key = (blockno << 8) | id;
        }
        else
        {
            __set_errno(ENOENT);
        }
    }
    else
    {
        __set_errno(EFAULT);
    }

    return (key);
}