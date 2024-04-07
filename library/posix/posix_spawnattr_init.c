/*
 * $Id: posix_posix_spawnattr_init.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawnattr_init(posix_spawnattr_t *attr) {
    _posix_spawnattr_t *psattrp = (_posix_spawnattr_t *)attr;
    int	err = 0;

    if ((*psattrp = (_posix_spawnattr_t)malloc(sizeof(struct _posix_spawnattr))) == NULL) {
        err = ENOMEM;
    } else {

        /*
         * The default value of this attribute shall be as if no
         * flags were set
         */
        (*psattrp)->psa_flags = 0;

        /*
         * The default value of this attribute shall be an empty
         * signal set
         */
        (*psattrp)->psa_sigdefault = 0;

        /* The default value of this attribute is unspecified */
        (*psattrp)->psa_sigmask = 0;

        /* The default value of this attribute shall be zero */
        (*psattrp)->psa_pgroup = 0;	/* doesn't matter */

        /* Default is no port actions to take */
        (*psattrp)->psa_ports = NULL;
    }

    return (err);
}