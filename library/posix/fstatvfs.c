/*
 * $Id: posix_fstatvfs.c,v 1.1 2024-07-04 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _POSIX_HEADERS_H
#include "posix_headers.h"
#endif /* _POSIX_HEADERS_H */

int
fstatvfs(int fd, struct statvfs *buf) {
    BPTR file;
    int result = -1;
    struct _clib4 *__clib4 = __CLIB4;
    struct InfoData *info = NULL;
    const char devicename[MAX_DOS_PATH];

    ENTER();

    SHOWVALUE(fd);
    SHOWPOINTER(buf);

    struct fd *fildes = __get_file_descriptor(__clib4, fd);
    if (fildes == NULL) {
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    file = DevNameFromFH(fildes->fd_File, devicename, MAX_DOS_PATH, DN_DEVICEONLY);
    if (file == BZERO) {
        __set_errno_r(__clib4, EINVAL);
        goto out;
    }
    D(("DeviceName: %s", devicename));

    info = AllocDosObject(DOS_INFODATA, TAG_END);
    if (info != NULL) {
        // 3 is the number of tags passed to GetDiskInfoTags call
        if (GetDiskInfoTags(
                GDI_StringNameInput, devicename,
                GDI_VolumeRequired, TRUE,
                GDI_InfoData, info,
                TAG_END) == 3) {
            uint32_t maxlength = STATVFS_MAX_NAME;

            FileSystemAttrTags(
                    FSA_MaxFileNameLengthR, &maxlength,
                    FSA_StringNameInput, file,
                    TAG_END);
            if (info->id_VolumeNode == BZERO) {
                /* Device not present or not responding */
                __set_errno_r(__clib4, ENXIO);
                goto out;
            }
            uint32 DosType = info->id_DiskType;

            __convert_info_to_statvfs(info, buf);

            buf->f_namemax = maxlength;
            /* Populate the missing statvfs structure */
            strncpy(buf->f_mntonname, devicename, 255);

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

                snprintf(buf->f_fstypename, 31, dosFormat, (DosType >> 24) & 0xFF, (DosType >> 16) & 0xFF, (DosType >> 8) & 0xFF, DosType & 0xFF);
            }
            else {
                /* If disk is not present set file system type to UNKNOWN */
                strcpy(buf->f_fstypename, "UNKNOWN");
            }
            result = 0;
        } else {
            LONG error = IoErr();
            D(("GetDiskInfoTags Error: %ld\n", error));
            __translate_io_error_to_errno(error);
        }
    }
    else {
        SHOWMSG("Could not allocate DOS_INFODATA object");
    }

out:
    if (info != NULL)
        FreeDosObject(DOS_INFODATA, info);

    RETURN(result);
    return (result);
}