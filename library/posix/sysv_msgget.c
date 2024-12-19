/*
 * $Id: sysv_msgget.c,v 1.2 2023-07-08 19:32:53 clib4devs Exp $
*/

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

#ifndef _IPC_HEADERS_H
#include "ipc_headers.h"
#endif /* _IPC_HEADERS_H */

/* Construct/Destroy queues */
static void *msg_construct(int key, int flags) {
    struct msqid_ds *qi;
    qi = calloc(1, sizeof(struct msqid_ds));
    if (qi) {
        qi->msg_perm.mode = flags & 0777;
        qi->msg_perm.key = key;
        qi->msg_first = 0;
        qi->msg_last = 0;
        qi->msg_cbytes = 0;
        qi->msg_qnum = 0;
        qi->msg_qbytes = DEFAULT_QSIZE;
        qi->msg_ctime = 0; /* XXX Should be set to "now" */
        qi->Lock = AllocSysObjectTags(ASOT_SEMAPHORE, TAG_DONE);
        qi->WList = AllocSysObjectTags(ASOT_LIST, ASOLIST_Min, TRUE, TAG_DONE);
        qi->RList = AllocSysObjectTags(ASOT_LIST, ASOLIST_Min, TRUE, TAG_DONE);
    }
    return (qi);
}

int
_msgget(key_t key, int flags) {
    ENTER();
    int ret = -1;

    SHOWVALUE(key);
    SHOWVALUE(flags);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    IPCLock(&res->msgcx.keymap);
    ret = GetIPCKeyId(&res->msgcx.keymap, key, flags, (void *(*)(int, int)) msg_construct);
    if (ret < 0) {
        __set_errno(-ret);
        ret = -1;
    }
    IPCUnlock(&res->msgcx.keymap);

    RETURN(ret);
    return ret;
}
