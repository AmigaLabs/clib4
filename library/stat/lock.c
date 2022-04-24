/*
 * $Id: stat_lock.c,v 1.1 2006-11-13 09:25:28 clib2devs Exp $
*/

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

/*
 * __lock(): An implementation of Lock() which remembers whether or
 *           not it resolved soft links.
 *
 * Unfortunately is is limited to 255 character names.
 */

BPTR
__lock(
        const char *name,
        const int mode,
        int *link_length,
        char *real_name,
        size_t real_name_size) {
    D_S(struct bcpl_name, bname);
    const size_t name_size = sizeof(bname->name);
    char *new_name = NULL;
    struct DevProc *dvp = NULL;
    BPTR lock = ZERO;
    size_t name_len;
    LONG error;

    assert(name != NULL && link_length != NULL);

    if (real_name != NULL && real_name_size > 0)
        strcpy(real_name, "");

    name_len = strlen(name);
    if (name_len >= name_size) {
        SetIoErr(ERROR_LINE_TOO_LONG);
        goto out;
    }

    /* Convert the name into a BCPL string. */
    bname->name[0] = name_len;
    memmove(&bname->name[1], name, name_len);

    while (TRUE) {
        /* Get a handle on the device, volume or assignment name in the path. */
        dvp = GetDeviceProc((STRPTR) name, dvp);
        if (dvp == NULL)
            goto out;

        /* Try to obtain a lock on the object. */
        lock = DoPkt(dvp->dvp_Port, ACTION_LOCATE_OBJECT, dvp->dvp_Lock, MKBADDR(bname), mode, 0, 0);
        if (lock != ZERO)
            break;

        error = IoErr();

        if (error == ERROR_OBJECT_NOT_FOUND) {
            /* If this is part of a multi-volume assignment, try the next part. */
            if (FLAG_IS_SET(dvp->dvp_Flags, DVPF_MULTIASSIGN))
                continue;

            /* Not much we can do here... */
            break;
        } else if (error == ERROR_IS_SOFT_LINK) {
            size_t new_name_size = name_size + 1;
            LONG result;

            /* Provide as much buffer space as possible. */
            if (real_name_size > new_name_size)
                new_name_size = real_name_size;

            /* For soft link resolution we need a temporary buffer to
               let the file system store the resolved path name in. */
            new_name = malloc(new_name_size);
            if (new_name == NULL) {
                SetIoErr(ERROR_NO_FREE_STORE);
                goto out;
            }

            /* Now ask the file system to resolve the entire path. */
            result = ReadSoftLink(dvp->dvp_Port, dvp->dvp_Lock, (STRPTR) name, (STRPTR) new_name, (LONG) new_name_size);
            if (result < 0) {
                /* This will return either -1 (resolution error) or -2
                   (buffer too small). We regard both as trouble. */
                SetIoErr(ERROR_INVALID_COMPONENT_NAME);
                goto out;
            }

            assert(result > 0);

            /* Remember the length of the link name. */
            (*link_length) = result;

            /* If the caller supplied a buffer, copy as much of the name
               as possible into it. */
            if (real_name != NULL && real_name_size > 0)
                strlcpy(real_name, new_name, real_name_size);

            /* Finished for now. */
            break;
        } else {
            /* Some other error; ask if the user wants to have another go at it. */
            if (ErrorReport(error, REPORT_LOCK, dvp->dvp_Lock, dvp->dvp_Port) != 0)
                break;
        }

        /* Retry the lookup. */
        FreeDeviceProc(dvp);
        dvp = NULL;
    }

out:

    error = IoErr();

    if (new_name != NULL)
        free(new_name);

    if (dvp != NULL)
        FreeDeviceProc(dvp);

    SetIoErr(error);

    return (lock);
}
