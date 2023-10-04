/*
 * $Id: unistd_chdir_exit.c,v 1.7 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

CLIB_DESTRUCTOR(__chdir_exit) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->__current_directory_changed) {
        BPTR old_dir;

        old_dir = SetCurrentDir(__clib4->__original_current_directory);
        __clib4->__original_current_directory = BZERO;

        if (__clib4->__unlock_current_directory) {
            UnLock(old_dir);

            __clib4->__unlock_current_directory = FALSE;
        }

        __clib4->__current_directory_changed = FALSE;
    }

    LEAVE();
}
