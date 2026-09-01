/*
 * $Id: stat_umask.c,v 1.7 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

mode_t __current_umask = S_IWGRP | S_IWOTH;

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

mode_t
umask(mode_t new_mask) {
    mode_t result;

    ENTER();

    SHOWVALUE(new_mask);

    /* usergroup.library holds the real mask, but we mirror it in
     * __current_umask, so umask() keeps working without the library. */
    if (__ensure_usergroup_library(__CLIB4)) {
        result = __getumask();

        __current_umask = new_mask & (S_IRWXU | S_IRWXG | S_IRWXO);

        __umask(__current_umask);
    } else {
        result = __current_umask;

        __current_umask = new_mask & (S_IRWXU | S_IRWXG | S_IRWXO);
    }

    RETURN(result);
    return (result);
}
