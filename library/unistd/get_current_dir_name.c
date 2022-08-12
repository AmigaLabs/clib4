/*
 * $Id: unistd_get_current_dir_name.c,v 1.0 2022-05-06 09:35:04 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

char *
get_current_dir_name(void) {
    char *buffer = malloc(PATH_MAX);
    if (buffer) {
        char *tmpbuf = getcwd(buffer, PATH_MAX);
        if (tmpbuf != NULL) {
            return buffer;
        }
        free(buffer);
        buffer = NULL;
    }

    return NULL;
}
