/*
 * $Id: posix_statvfs.c,v 1.0 2022-03-28 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _POSIX_HEADERS_H
#include "posix_headers.h"
#endif /* _POSIX_HEADERS_H */

int
statvfs(const char *path, struct statvfs *buf) {
    struct name_translation_info path_name_nti;
    int result = -1;
    struct _clib4 *__clib4 = __CLIB4;
    struct InfoData *info = NULL;

    ENTER();

    SHOWSTRING(path);
    SHOWPOINTER(buf);

    if (__clib4->__unix_path_semantics) {
        if (__translate_unix_to_amiga_path_name(&path, &path_name_nti) != 0)
            goto out;

        if (path_name_nti.is_root) {
            __set_errno_r(__clib4, EINVAL);
            goto out;
        }
    }

    info = AllocDosObject(DOS_INFODATA, 0);
    if (info != NULL) {
        // 3 is the number of tags passed to GetDiskInfoTags call
        if (GetDiskInfoTags(
                GDI_StringNameInput, path,
                GDI_VolumeRequired, TRUE,
                GDI_InfoData, info,
                TAG_END) == 3) {
            uint32_t maxlength = STATVFS_MAX_NAME;
            FileSystemAttrTags(
                    FSA_MaxFileNameLengthR, &maxlength,
                    FSA_StringNameInput, path,
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
            strncpy(buf->f_mntonname, path, 255);

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
            __translate_io_error_to_errno(IoErr());
        }
    } else
        __set_errno_r(__clib4, ENOMEM);

out:
    if (info != NULL)
        FreeDosObject(DOS_INFODATA, info);

    RETURN(result);
    return (result);
}