/*
 * $Id: posix_posix_spawn_file_actions_init.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions) {
    _posix_spawn_file_actions_t *psactsp = (_posix_spawn_file_actions_t *) file_actions;
    int err = 0;

    if ((*psactsp = (_posix_spawn_file_actions_t) malloc(PSF_ACTIONS_SIZE(PSF_ACTIONS_INIT_COUNT))) == NULL) {
        err = ENOMEM;
    } else {
        (*psactsp)->psfa_act_alloc = PSF_ACTIONS_INIT_COUNT;
        (*psactsp)->psfa_act_count = 0;
    }

    return (err);
}