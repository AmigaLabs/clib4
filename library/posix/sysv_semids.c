/*
 * $Id: sysv_semids.c,v 1.1 2023-07-08 17:07:00 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

long int
_semids(long int *buf, uint32_t nids, long unsigned int *idcnt) {
    ENTER();
    int32 ret = -1;

    SHOWPOINTER(buf);
    SHOWVALUE(nids);
    SHOWPOINTER(idcnt);

    struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    IPCLock(&res->semcx.keymap);
    ret = IPCids(&res->semcx.keymap, buf, nids, idcnt);
    IPCUnlock(&res->semcx.keymap);
    
    RETURN(ret);
    return ret;
}
