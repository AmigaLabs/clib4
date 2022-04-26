/*
 * $Id: unistd_translaterel.c,v 1.5 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
__translate_relative_path_name(char const **name_ptr, char *replacement_buffer, size_t replacement_buffer_size) {
    int result = ERROR;
    char *name;

    assert(name_ptr != NULL && (*name_ptr) != NULL && replacement_buffer != NULL);

    /* If we have a current directory which all references should
     * be made relative to, do just that. Absolute paths
     * are not modified, though.
     */
    name = (char *) (*name_ptr);
    if (__current_path_name[0] != '\0' && name[0] != '/') {
        size_t total_len;
        BOOL add_slash;

        /* Get the current directory name and get
         * ready to attach the file name at the end.
         */
        total_len = strlen(__current_path_name);

        if (__current_path_name[total_len - 1] != '/' &&
            __current_path_name[total_len - 1] != ':' &&
            name[0] != '\0') {
            add_slash = TRUE;

            total_len++;
        } else {
            add_slash = FALSE;
        }

        total_len += strlen(name);

        /* Check if the complete string will fit, including the terminating NUL byte. */
        if (total_len + 1 > replacement_buffer_size) {
            __set_errno(ENAMETOOLONG);
            goto out;
        }

        /* Put the file name together. */
        strcpy(replacement_buffer, __current_path_name);

        if (add_slash)
            strcat(replacement_buffer, "/");

        strcat(replacement_buffer, name);

        assert(strlen(replacement_buffer) < replacement_buffer_size);

        (*name_ptr) = replacement_buffer;
    }

    result = OK;

out:

    return (result);
}
