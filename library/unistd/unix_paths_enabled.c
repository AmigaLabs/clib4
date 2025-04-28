/*
 * $Id: unistd_unix_paths_enabled.c,v 1.0 2025-28-08 14:39:23 clib4devs Exp $
*/

#include <dos.h>
#include <unistd.h>

int
unixPathsEnabled(void) {
    struct _clib4 *__clib4 = __CLIB4;
    return __clib4->__unix_path_semantics;
}