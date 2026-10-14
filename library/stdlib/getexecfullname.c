/*
* $Id: stdlib_getexecfullname.c,v 1.0 2026-10-14 13:28:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

const char *
getexecfullname(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    static char current_full_name[MAXPATHLEN];
    char dir_name[MAXPATHLEN] = {0};
    BPTR dir_lock = BZERO;

    /* The directory of the executable is not the current directory: ask DOS
     * where the program was loaded from (Workbench: from the startup message). */
    if (__clib4->__WBenchMsg != NULL)
        dir_lock = __clib4->__WBenchMsg->sm_ArgList[0].wa_Lock;
    else
        dir_lock = GetProgramDir(); /* shared lock, must not be unlocked; BZERO if none */

    if (dir_lock == BZERO ||!NameFromLock(dir_lock, (STRPTR) dir_name, sizeof(dir_name)))
        strlcpy(dir_name, __clib4->__current_path_name, sizeof(dir_name));

    /* __progname may contain a path (e.g. "Work:Clib4Tests/exec_test2"): keep only the file name */
    const char *base_name = __clib4->__progname;
    const char *slash = strrchr(base_name, '/');
    const char *colon = strrchr(base_name, ':');
    if (slash != NULL && (colon == NULL || slash > colon))
        base_name = slash + 1;
    else if (colon != NULL)
        base_name = colon + 1;

    size_t dir_len = strlen(dir_name);
    BOOL needs_separator = dir_len > 0 && dir_name[dir_len - 1] != ':' && dir_name[dir_len - 1] != '/';

    snprintf(current_full_name, sizeof(current_full_name), "%s%s%s", dir_name, needs_separator ? "/" : "", base_name);

    RETURN((const char *) current_full_name);
    return current_full_name;
}
