/*
 * $Id: posix_basename.c,v 1.8 2006-10-02 07:15:37 clib2devs Exp $
*/

#include <string.h>
#include <libgen.h>
#include <stdio.h>

#include "debug.h"

char *
basename(const char *path) {
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
    if (path == NULL || path[0] == '\0') {
        str = ".";
        len = 1;
    } else {
        /* Strip all trailing slashes. */
        len = strlen(path);
        while (len > 0 && path[len - 1] == '/')
            len--;

        /* Is there anything left? */
        if (len > 0) {
            size_t i;

            /* Return what follows the last slash in the path. That's
               usually a file or directory name. */
            str = path;

            for (i = len - 1;; i--) {
                if (path[i] == '/') {
                    len -= i + 1;

                    str = &path[i + 1];
                    break;
                }

                if (i == 0)
                    break;
            }
        }

        /* If the whole operation produced an empty string, then it
           means that we dealt with a string which consisted entirely
           of slashes. And that's what we will return. */
        if (len == 0) {
            str = "/";
            len = 1;
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
