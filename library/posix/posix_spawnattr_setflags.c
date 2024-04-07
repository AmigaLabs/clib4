/*
 * $Id: spawn_posix_spawnattr_setflags.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags) {
    _posix_spawnattr_t psattr;

    if (attr == NULL || *attr == NULL)
        return EINVAL;

    psattr = *(_posix_spawnattr_t *) attr;
    psattr->psa_flags = flags;

    return (0);
}