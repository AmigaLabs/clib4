/*
 * $Id: posix_getvfsstat.c,v 1.0 2025-02-03 16:35:27 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _POSIX_HEADERS_H
#include "posix_headers.h"
#endif /* _POSIX_HEADERS_H */

int
getvfsstat(struct statvfs *buf, size_t bufsize, int flags) {
    struct _clib4 *__clib4 = __CLIB4;
    int count = 0;
    uint32_t lFlags = LDF_DEVICES | LDF_READ;

    /* On OS4 we don't have any difference between ST_WAIT and ST_NOWAIT */
    if (flags != ST_WAIT && flags != ST_NOWAIT) {
        __set_errno_r(__clib4, EINVAL);
        return -1;
    }

    struct DosList *dosList = LockDosList(lFlags);
    if (dosList == NULL) {
        __set_errno_r(__clib4, ENOMEM);
        return -1;
    }

    struct InfoData *info = AllocDosObject(DOS_INFODATA, 0);
    if (info == NULL) {
        __set_errno_r(__clib4, ENOMEM);
        UnLockDosList(LDF_DEVICES | LDF_READ);
        return -1;
    }

    // Traverse the DOS list
    while ((dosList = NextDosEntry(dosList, lFlags))) {
        if (dosList->dol_Port != NULL) {
            /* If buf is NOT NULL just get all information from DosList entry and increase the count */
            if (buf != NULL) {
                // Check if the buffer has space
                if (count >= bufsize / sizeof(struct statvfs)) {
                    break;
                }

                if (GetDiskInfoTags(
                        GDI_MsgPortInput, dosList->dol_Port,
                        GDI_InfoData, info,
                        TAG_END) != 0) {

                    uint32 DosType = info->id_DiskType;

                    /* Convert the InfoData to statvfs structure */
                    __convert_info_to_statvfs(info, &buf[count]);
                    /* Populate the missing statvfs structure */
                    CopyStringBSTRToC(dosList->dol_Name, buf[count].f_mntonname, sizeof(buf[count].f_mntonname));

                    /* Skip DOSType checking if Disk is not present (for example on ICD0, IDF0 and so on..) */
                    if (info->id_DiskType != ID_NO_DISK_PRESENT) {
                        const char *dosFormat = "%c%c%c\\%02lx";
                        if ((DosType & 0xFF) > 0x20) {
                            dosFormat = "%c%c%c%c";
                        }
                        if (!(DosType & 0xFF000000))
                            DosType |= 0x20000000;
                        else if (!(DosType & 0x00FF0000))
                            DosType |= 0x00200000;
                        else if (!(DosType & 0x0000FF00))
                            DosType |= 0x00002000;

                        sprintf(buf[count].f_fstypename, dosFormat, (DosType >> 24) & 0xFF, (DosType >> 16) & 0xFF,
                                (DosType >> 8) & 0xFF, DosType & 0xFF);
                    }
                    else {
                        /* If disk is not present set file system type to UNKNOWN */
                        strcpy(buf[count].f_fstypename, "UNKNOWN");
                    }

                    count++;
                }
            }
            else {
                /* If buf is NULL just try to get Disk Info from DosList entry and increase the count */
                if (GetDiskInfoTags(
                        GDI_MsgPortInput, dosList->dol_Port,
                        GDI_InfoData, info,
                        TAG_END) != 0) {
                    count++;
                }
            }
        }
    }
    FreeDosObject(DOS_INFODATA, info);

    // Unlock the DOS list
    UnLockDosList(lFlags);

    return count;
}