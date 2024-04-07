/*
 * $Id: posix_posix_spawnattr_getschedpolicy.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawnattr_getschedpolicy(const posix_spawnattr_t *__restrict sa, int *__restrict schedpolicy) {
    _posix_spawnattr_t *psattrp = (_posix_spawnattr_t *)sa;

    *schedpolicy = (*psattrp)->psa_schedpolicy;
    return (0);
}