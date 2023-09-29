/*
 * $Id: libgen_dirname.c,v 1.8 2006-10-02 07:15:37 clib4devs Exp $
*/

#include <string.h>
#include <libgen.h>
#include <stdio.h>

#include "debug.h"

char *
dirname(const char *path) {
    static char new_path[MAXPATHLEN];
    const char *str;
    char *result;
    size_t len;

    ENTER();

    if (path == NULL)
        SHOWPOINTER(path);
    else
        SHOWSTRING(path);

    /* An empty path always comes out as the "current directory". */
    str = ".";
    len = 1;

    if (path != NULL && path[0] != '\0') {
        /* Strip all trailing slashes. */
        len = strlen(path);
        while (len > 1 && path[len - 1] == '/')
            len--;

        /* Is there anything left? */
        if (len > 0) {
            size_t i;

            for (i = len - 1;; i--) {
                if (path[i] == '/') {
                    /* Return everything up to, but not including
                       the last slash in the path. That's usually
                       the directory name. */
                    str = path;
                    len = i;

                    /* If that produces an empty string, it means
                       that the entire string consists of slash
                       characters. We'll return only the first. */
                    if (i == 0)
                        len++;

                    break;
                }

                if (i == 0)
                    break;
            }
        }
    }

    /* Truncate the path name we can return. This function always returns
       a valid pointer rather than NULL because some software expects it
       to do so (I blame the specifications). */
    if (len >= sizeof(new_path))
        len = sizeof(new_path) - 1;

    memcpy(new_path, str, len);
    new_path[len] = '\0';

    result = new_path;

    RETURN(result);
    return (result);
}
