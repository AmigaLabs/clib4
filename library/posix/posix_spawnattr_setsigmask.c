/*
 * $Id: posix_posix_spawnattr_setsigmask.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawnattr_setsigmask(posix_spawnattr_t *__restrict attr, const sigset_t *__restrict sigmask) {
    _posix_spawnattr_t psattr;

    if (attr == NULL || *attr == NULL)
        return EINVAL;

    psattr = *(_posix_spawnattr_t *) attr;
    psattr->psa_sigmask = *sigmask;

    return (0);
}