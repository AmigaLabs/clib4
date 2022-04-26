/*
 * $Id: mman_mmap.c,v 1.0 2021-01-18 20:17:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>

void *
mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
    (void) (prot);
    (void) (flags);

    ENTER();

    SHOWPOINTER(addr);
    SHOWVALUE(len);
    SHOWVALUE(prot);
    SHOWVALUE(flags);
    SHOWVALUE(fd);
    SHOWVALUE(offset);

    if (fd > 0)
    {
        if (addr != NULL)
        {
            if ((addr = calloc(1, len)))
            {
                lseek(fd, offset, SEEK_SET);
                read(fd, addr, len);
            }

            if (!addr)
            {
                __set_errno(ENOMEM);
                RETURN(NULL);
                return NULL;
            }

            RETURN(addr);
            return addr;
        }
        else
        {
            void *data = calloc(1, len);
            if (data != NULL)
            {
                lseek(fd, offset, SEEK_SET);
                read(fd, data, len);
            }

            if (!data)
            {
                __set_errno(ENOMEM);
                RETURN(NULL);
                return NULL;
            }
            RETURN(data);
            return data;
        }
    }
    else
    {
        if ((addr = calloc(1, len))) {
            RETURN(addr);
            return addr;
        }
        else
        {
            __set_errno(ENOMEM);
            RETURN(NULL);
            return NULL;
        }
    }
}