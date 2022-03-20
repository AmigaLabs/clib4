/*
 * $Id: sys_semtimedop.c,v 1.00 2021-02-02 17:26:15 clib2devs Exp $
*/

#ifdef HAVE_SYSV

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

int
_semtimedop(int semid, const struct sembuf *ops, int nops, struct timespec *to) {
    DECLARE_SYSVYBASE();

    int ret = -1;
    if (__global_clib2->haveShm) {
        ret = semtimedop(semid, ops, nops, to);
        if (ret < 0) {
            __set_errno(GetIPCErr());
        }
    } else {
        __set_errno(ENOSYS);
    }

    return ret;
}

#endif