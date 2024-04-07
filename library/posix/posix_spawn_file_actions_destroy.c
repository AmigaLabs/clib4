/*
 * $Id: posix_posix_spawn_file_actions_destroy.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions) {
    _posix_spawn_file_actions_t psacts;

    if (file_actions == NULL || *file_actions == NULL)
        return EINVAL;

    psacts = *(_posix_spawn_file_actions_t *) file_actions;
    free(psacts);
    *file_actions = NULL;

    return (0);
}