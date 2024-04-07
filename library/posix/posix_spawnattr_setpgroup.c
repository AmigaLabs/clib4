/*
 * $Id: posix_posix_spawnattr_setpgroup.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup) {
    _posix_spawnattr_t psattr;

    if (attr == NULL || *attr == NULL)
        return EINVAL;

    psattr = *(_posix_spawnattr_t *) attr;
    psattr->psa_pgroup = pgroup;

    return (0);
}