/*
 * $Id: posix_posix_spawn.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "spawn_internal.h"

int
posix_spawn(pid_t *__restrict pid, const char *__restrict path,
            posix_spawn_file_actions_t *file_actions,
            const posix_spawnattr_t *attrp,
            char *const argv[__restrict], char *const envp[__restrict]) {
    int saveerrno = errno;
    int ret;
    /*
     * Only do extra work if we have file actions or attributes to push
     * down.  We use a descriptor to push this information down, since we
     * want to have size information, which will let us (1) preallocate a
     * single chunk of memory for the copyin(), and (2) allow us to do a
     * single copyin() per attributes or file actions as a monlithic block.
     *
     * Note:	A future implementation may attempt to do the same
     *		thing for the argv/envp data, which could potentially
     *		result in a performance improvement due to increased
     *		kernel efficiency, even though it would mean copying
     *		the data in user space.
     */
    if ((file_actions != NULL && (*file_actions != NULL) &&
         (*(_posix_spawn_file_actions_t *) file_actions)->psfa_act_count > 0) || attrp != NULL) {
        struct _posix_spawn_args_desc ad;

        memset(&ad, 0, sizeof(ad));
        if (attrp != NULL && *attrp != NULL) {
            _posix_spawnattr_t psattr = *(_posix_spawnattr_t *) attrp;
            ad.attr_size = sizeof(struct _posix_spawnattr);
            ad.attrp = psattr;

            if (psattr->psa_ports != NULL) {
                ad.port_actions = psattr->psa_ports;
                ad.port_actions_size = PS_PORT_ACTIONS_SIZE(ad.port_actions->pspa_count);
            }
        }
        if (file_actions != NULL && *file_actions != NULL) {
            _posix_spawn_file_actions_t psactsp = *(_posix_spawn_file_actions_t *) file_actions;

            if (psactsp->psfa_act_count > 0) {
                ad.file_actions_size = PSF_ACTIONS_SIZE(psactsp->psfa_act_count);
                ad.file_actions = psactsp;
            }
        }

        ret = __posix_spawn(pid, path, &ad, argv, envp);
    } else
        ret = __posix_spawn(pid, path, NULL, argv, envp);

    if (ret < 0)
        ret = errno;
    errno = saveerrno;
    return ret;
}