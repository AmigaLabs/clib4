/*
* $Id: backtrace_symbols.c,v 1.0 2025-03-02 12:06:14 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <libraries/elf.h>
#include <proto/elf.h>

#include <execinfo.h>

char **
backtrace_symbols(void *const *buffer, int size) {
    struct _clib4 *__clib4 = __CLIB4;

    if (size <= 0)
        return NULL;

    DECLARE_DEBUGBASE();

    // Allocate array of strings
    char **symbols = __malloc_r(__clib4, size * sizeof(char *));
    if (!symbols)
        return NULL;

    for (int i = 0; i < size; i++) {
        // Format: "<hex-addr> <func>+<offset> [<lib>]"
        uint32 addr = (uint32) buffer[i];
        struct DebugSymbol *symbol = ObtainDebugSymbol((APTR) addr, NULL);
        if (symbol) {
            const int max_len = 256;
            symbols[i] = __calloc_r(__clib4, 1, max_len);
            if (symbols[i]) {

                switch (symbol->Type) {
                    case DEBUG_SYMBOL_68K_MODULE:
                        snprintf(symbols[i], max_len, "%p %s+0x%lx [68k module]", addr, symbol->Name, symbol->Offset);
                        break;
                    case DEBUG_SYMBOL_NATIVE_MODULE:
                        snprintf(symbols[i], max_len, "%p %s+0x%lx [native module]", addr, symbol->Name, symbol->Offset);
                        break;
                    case DEBUG_SYMBOL_KERNEL_MODULE:
                        snprintf(symbols[i], max_len, "%p %s+0x%lx [kernel module]", addr, symbol->Name, addr);
                        break;
                    case DEBUG_SYMBOL_MODULE:
                    case DEBUG_SYMBOL_MODULE_STABS:
                        snprintf(
                                symbols[i],
                                max_len,
                                "%p %s+0x%lx [%s]",
                                buffer[i],
                                symbol->Name ? symbol->Name : "???",
                                symbol->Offset,
                                symbol->SourceFileName ? symbol->SourceFileName : ""
                        );
                        break;
                    default:
                        // Fallback to address
                        snprintf(symbols[i], 20, "%p", buffer[i]);
                        break;
                }
            }
            ReleaseDebugSymbol(symbol);
		}
        else {
            // Fallback: Just the address
            symbols[i] = __calloc_r(__clib4, 1, 20); // Enough for "0x<hex>"
            if (symbols[i]) {
                snprintf(symbols[i], 20, "%p", buffer[i]);
            }
        }
    }

    return symbols;
}