/*
 * $Id: ipc_ftok.c,v 1.1 2023-07-08 12:19:14 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <sys/ipc.h>

key_t ftok(const char *path, int id) {
    key_t key = (key_t) - 1;
    BPTR lock = 0;
    uint32_t blockno = 0;
    struct Lock *flock;
    struct name_translation_info path_name_nti;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(path);
    SHOWVALUE(id);

    if (__clib4->__unix_path_semantics) {
        if (__translate_unix_to_amiga_path_name(&path, &path_name_nti) != 0)
            return key;
        if (path_name_nti.is_root) {
            __set_errno(EACCES);
            return -1;
        }
    }

    if (path) {
        lock = Lock(path, SHARED_LOCK);

        if (lock) {
            flock = BADDR(lock);
            blockno = flock->fl_Key;
            UnLock(lock);
            key = (blockno << 8) | id;
        } else {
            __set_errno(ENOENT);
        }
    } else {
        __set_errno(EFAULT);
    }

    RETURN(key);
    return (key);
}