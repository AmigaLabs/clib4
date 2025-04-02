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

char **
backtrace_symbols(void *const *buffer, int size) {
    struct _clib4 *__clib4 = __CLIB4;

    if (size <= 0)
        return NULL;
    if (!__clib4->__dl_root_handle != 0)
        return NULL;

    struct ElfIFace *IElf = __clib4->IElf;

    struct Hook symbol_hook;
    symbol_hook.h_Entry = (ULONG (*)())amigaos_symbols_callback;
    symbol_hook.h_Data =  NULL;
    ScanSymbolTable(__clib4->__dl_root_handle, &symbol_hook, NULL);

    // Allocate array of strings
    char **symbols = __malloc_r(__clib4, size * sizeof(char *));
    if (!symbols)
        return NULL;

    struct Elf32_SymbolQuery query;
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
            // Format: "<hex-addr> <func>+<offset> [<lib>]"
            const int max_len = 256;
            symbols[i] = __calloc_r(__clib4, 1, max_len);
            if (symbols[i]) {
    			found = TRUE;
                GetElfAttrsTags(__clib4->__dl_root_handle, EAT_FileName, &fileBuffer, TAG_DONE);
                snprintf(
                    symbols[i],
		            max_len,
		            "%p %s+0x%lx [%s]",
                    buffer[i],
                    query.Name ? query.Name : "???",
                    query.Sym.st_value,
                    (STRPTR) fileBuffer
                );
            }
		}

        if (!found) {
            // Fallback: Just the address
            symbols[i] = __calloc_r(__clib4, 1, 20); // Enough for "0x<hex>"
            if (symbols[i]) {
                snprintf(symbols[i], 20, "%p", buffer[i]);
            }
        }
    }

    return symbols;
}