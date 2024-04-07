/*
 * $Id: spawn_posix_spawnattr_destroy.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawnattr_destroy(posix_spawnattr_t *attr) {
    _posix_spawnattr_t psattr;
    struct _clib4 *__clib4 = __CLIB4;

    if (attr == NULL || *attr == NULL)
        return EINVAL;

    psattr = *(_posix_spawnattr_t *) attr;
    if (__clib4->posix_spawn_destroyportactions_np != NULL)
        __clib4->posix_spawn_destroyportactions_np(attr);

    free(psattr);
    *attr = NULL;

    return (0);
}