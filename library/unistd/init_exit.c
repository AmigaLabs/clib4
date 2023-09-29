/*
 * $Id: unistd_init_exit.c,v 1.14 2006-09-27 09:40:06 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

CLIB_CONSTRUCTOR(unistd_init) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    NewList((struct List *)&__clib4->__unlink_list);
    InitSemaphore(&__clib4->__unlink_semaphore);

    LEAVE();

    CONSTRUCTOR_SUCCEED();
}

CLIB_DESTRUCTOR(unistd_exit) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->__unlink_list.mlh_Head != NULL && NOT IsMinListEmpty(&__clib4->__unlink_list)) {
        struct UnlinkNode *uln;
        BPTR old_dir;

        /* Close all the files that still might be open. */
        __close_all_files(__clib4);

        while ((uln = (struct UnlinkNode *) RemHead((struct List *) &__clib4->__unlink_list))) {
            D(("deleting '%s'", uln->uln_Name));

            old_dir = SetCurrentDir(uln->uln_Lock);
            Delete(uln->uln_Name);
            SetCurrentDir(old_dir);

            UnLock(uln->uln_Lock);
        }
    }

    LEAVE();
}
