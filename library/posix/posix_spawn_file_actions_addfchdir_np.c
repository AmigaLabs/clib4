/*
 * $Id: posix_posix_spawn_file_actions_addfchdir_np.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawn_file_actions_addchdir_np(posix_spawn_file_actions_t *file_actions, const char *__restrict path) {
    _posix_spawn_file_actions_t *psactsp;
    _psfa_action_t *psfileact;

    if (file_actions == NULL || *file_actions == NULL)
        return EINVAL;

    psactsp = (_posix_spawn_file_actions_t *) file_actions;

    /* If we do not have enough slots, grow the structure */
    if ((*psactsp)->psfa_act_count == (*psactsp)->psfa_act_alloc) {
        /* need to grow file actions structure */
        if (_posix_spawn_file_actions_grow(psactsp))
            return ENOMEM;
    }

    /*
     * Allocate next available slot and fill it out
     */
    psfileact = &(*psactsp)->psfa_act_acts[(*psactsp)->psfa_act_count++];

    psfileact->psfaa_type = PSFA_CHDIR;
    psfileact->psfaa_path = strdup(path);
    if (psfileact->psfaa_path == NULL) {
        free(psfileact);
        return errno;
    }

    return 0;
}
