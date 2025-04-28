/*
* $Id: backtrace_symbols.c,v 1.0 2025-03-02 12:06:14 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <libraries/elf.h>
#include <proto/elf.h>

#include <execinfo.h>

void
backtrace_symbols_fd(void *const *buffer, int size, int fd) {
    struct _clib4 *__clib4 = __CLIB4;

    if (size <= 0)
        return;

    DECLARE_DEBUGBASE();

    for (int i = 0; i < size; i++) {
        // Format: "<hex-addr> <func>+<offset> [<lib>]"
        uint32 addr = (uint32) buffer[i];
        struct DebugSymbol *symbol = ObtainDebugSymbol((APTR) addr, NULL);
        if (symbol) {
            const int max_len = 256;
            char line[256];
            int len = 0;

            switch (symbol->Type) {
                case DEBUG_SYMBOL_68K_MODULE:
                    len = snprintf(line, max_len, "%p %s+0x%lx [68k module]\n", addr, symbol->Name, symbol->Offset);
                    break;
                case DEBUG_SYMBOL_NATIVE_MODULE:
                    len = snprintf(line, max_len, "%p %s+0x%lx [native module]\n", addr, symbol->Name, symbol->Offset);
                    break;
                case DEBUG_SYMBOL_KERNEL_MODULE:
                    len = snprintf(line, max_len, "%p %s+0x%lx [kernel module]\n", addr, symbol->Name, addr);
                    break;
                case DEBUG_SYMBOL_MODULE:
                case DEBUG_SYMBOL_MODULE_STABS:
                    len = snprintf(
                            line,
                            max_len,
                            "%p %s+0x%lx [%s]\n",
                            buffer[i],
                            symbol->Name ? symbol->Name : "???",
                            symbol->Offset,
                            symbol->SourceFileName ? symbol->SourceFileName : ""
                    );
                    break;
                default:
                    // Fallback to address
                    len = snprintf(line, 20, "%p", buffer[i]);
                    break;
            }
            write(fd, line, len);
            ReleaseDebugSymbol(symbol);
        }
        else {
            // Fallback: Just the address
            char line[20];
            int len = snprintf(line, sizeof(line), "%p\n", buffer[i]);
            write(fd, line, len);
        }
    }
}