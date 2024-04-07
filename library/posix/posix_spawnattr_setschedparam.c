/*
 * $Id: posix_posix_spawnattr_setschedparam.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawnattr_setschedparam(posix_spawnattr_t *__restrict sa, const struct sched_param *__restrict schedparam) {
    _posix_spawnattr_t *psattrp = (_posix_spawnattr_t *)sa;

    (*psattrp)->psa_schedparam = *schedparam;
    return 0;
}