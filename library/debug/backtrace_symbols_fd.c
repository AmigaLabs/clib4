/*
* $Id: backtrace_symbols.c,v 1.0 2025-03-02 12:06:14 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <libraries/elf.h>
#include <proto/elf.h>

#include <execinfo.h>

static ULONG amigaos_symbols_callback(struct Hook *hook, struct Task *task, struct SymbolMsg *symbolmsg) {
    return 1;
}

void
backtrace_symbols_fd(void *const *buffer, int size, int fd) {
    struct _clib4 *__clib4 = __CLIB4;

    if (size <= 0)
        return;
    if (!__clib4->__dl_root_handle != 0)
        return;

    struct ElfIFace *IElf = __clib4->IElf;

    struct Hook symbol_hook;
    symbol_hook.h_Entry = (ULONG (*)())amigaos_symbols_callback;
    symbol_hook.h_Data =  NULL;
    ScanSymbolTable(__clib4->__dl_root_handle, &symbol_hook, NULL);

    struct Elf32_SymbolQuery query;
    query.Flags = ELF32_SQ_BYVALUE | ELF32_SQ_LOAD;
    for (int i = 0; i < size; i++) {
	    BOOL found = FALSE;
        char nameBuffer[256] = {0};
	    ULONG fileBuffer;

        query.Flags = ELF32_SQ_BYVALUE | ELF32_SQ_LOAD;
        query.Value = (uint32) buffer[i];
        query.NameLength = 255;
        query.Name = nameBuffer;

        SymbolQuery(__clib4->__dl_root_handle, 1, &query);
        if (query.Found) {
			found = TRUE;
            char line[256];
            GetElfAttrsTags(__clib4->__dl_root_handle, EAT_FileName, &fileBuffer, TAG_DONE);
            int len = snprintf(
                line, sizeof(line), "%p %s+0x%lx [%s]\n",
                buffer[i],
                query.Name ? query.Name : "???",
                query.Sym.st_value,
                (STRPTR) fileBuffer
            );
            write(fd, line, len);
		}

        if (!found) {
            char line[20];
            int len = snprintf(line, sizeof(line), "%p\n", buffer[i]);
            write(fd, line, len);
        }
    }
}