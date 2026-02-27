/*
 * $Id: stdlib_dladdr.c,v 1.0 2025-03-03 11:37:03 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <dlfcn.h>
#include <libraries/elf.h>

int
dladdr(const void *addr, Dl_info *info) {
    int result = -1;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWPOINTER(addr);
    SHOWPOINTER(info);

    if (info == NULL) {
        SHOWMSG("info is NULL");
        __set_errno_r(__clib4, EINVAL);
        return result;
    }

    DECLARE_DEBUGBASE();
    struct ElfIFace *IElf = __clib4->IElf;

    struct DebugSymbol *symbol = ObtainDebugSymbol((APTR) addr, NULL);
    if (symbol) {
        info->dli_fbase = NULL; // Should be got dynamically
        info->dli_fname = symbol->Name ? symbol->Name : "";
        info->dli_saddr = (void *) symbol->Offset;
    } else {
        __set_errno_r(__clib4, ENOSYS);
        RETURN(result);
        return (result);
    }

    if (__clib4->__dl_root_handle != NULL) {
        ULONG fileBuffer;

        GetElfAttrsTags(__clib4->__dl_root_handle, EAT_FileName, &fileBuffer, TAG_DONE);
        info->dli_sname = (STRPTR) fileBuffer;
    }
    else
        info->dli_fname = "";

    RETURN(result);
    return (result);
}
