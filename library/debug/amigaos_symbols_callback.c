#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

ULONG
amigaos_symbols_callback(struct Hook *hook, struct Task *task, struct SymbolMsg *symbolmsg) {
    return 1;
}
