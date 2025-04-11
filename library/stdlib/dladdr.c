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

    if (__clib4->__dl_root_handle != NULL) {
        struct ElfIFace *IElf = __clib4->IElf;
        struct Elf32_SymbolQuery query;

        struct Hook symbol_hook;
        symbol_hook.h_Entry = (ULONG (*)())amigaos_symbols_callback;
        symbol_hook.h_Data =  NULL;
        ScanSymbolTable(__clib4->__dl_root_handle, &symbol_hook, NULL);

        char nameBuffer[256] = {0};

        query.Flags = ELF32_SQ_BYVALUE | ELF32_SQ_LOAD;
        query.Value = (uint32) addr;
        query.NameLength = 255;
        query.Name = nameBuffer;
        SymbolQuery(__clib4->__dl_root_handle, 1, &query);
        if (query.Found) {
            ULONG fileBuffer;

            GetElfAttrsTags(__clib4->__dl_root_handle, EAT_FileName, &fileBuffer, TAG_DONE);
            info->dli_fname = (STRPTR) fileBuffer;
	        info->dli_fbase = NULL;
			//Printf("info->dli_fbase = %p\n", query.Sym.st_shndx);
		    info->dli_sname = nameBuffer;
		    info->dli_saddr = (void *) query.Sym.st_value;
	        result = 0;
	    }
    } else {
        __set_errno_r(__clib4, ENOSYS);
    }

    RETURN(result);
    return (result);
}
