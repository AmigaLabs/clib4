/*
 * $Id: posix_posix_spawn_file_actions_addopen.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *__restrict file_actions, int filedes, const char *__restrict path, int oflag, mode_t mode) {
    _posix_spawn_file_actions_t *psactsp;
    _psfa_action_t *psfileact;

    if (file_actions == NULL || *file_actions == NULL)
        return EINVAL;

    psactsp = (_posix_spawn_file_actions_t *) file_actions;
    /* Range check; required by POSIX */
    if (filedes < 0 || filedes >= OPEN_MAX)
        return (EBADF);

    /* If we do not have enough slots, grow the structure */
    if ((*psactsp)->psfa_act_count == (*psactsp)->psfa_act_alloc) {
        /* need to grow file actions structure */
        if (_posix_spawn_file_actions_grow(psactsp))
            return (ENOMEM);
    }

    /*
     * Allocate next available slot and fill it out
     */
    psfileact = &(*psactsp)->psfa_act_acts[(*psactsp)->psfa_act_count++];

    psfileact->psfaa_type = PSFA_OPEN;
    psfileact->psfaa_filedes = filedes;
    psfileact->psfaa_openargs.psfao_oflag = oflag;
    psfileact->psfaa_openargs.psfao_mode = mode;
    strlcpy(psfileact->psfaa_openargs.psfao_path, path, PATH_MAX);

    return (0);
}