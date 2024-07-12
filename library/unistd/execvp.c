/*
 * $Id: unistd_execvp.c,v 1.7 2006-09-25 14:51:15 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
execvp(const char *command, char *const argv[]) {
    char *command_buffer = NULL;
    size_t command_name_len, i;
    char *path_copy = NULL;
    int result = -1;
    BOOL found_path_separators;
    struct _clib4 *__clib4 = __CLIB4;

    /* Do not allow null command */
    if (command == NULL || (*command) == '\0') {
        __set_errno(ENOENT);
        goto out;
    }

    command_name_len = strlen(command);

    /* Check if there are any path separator characters in the
       command name. */
    found_path_separators = FALSE;

    for (i = 0; i < command_name_len; i++) {
        if (command[i] == '/' || command[i] == ':') {
            found_path_separators = TRUE;
            break;
        }
    }

    /* If it's an absolute or relative path name, it's easy. */
    if (found_path_separators) {
        result = execve(command, argv, __clib4->__environment);
    } else {
        size_t command_buffer_size = 0;
        const char *path_delimiter;
        char *path;
        const char *search_prefix;
        size_t search_prefix_len;
        size_t complete_path_len;
        int error;

        /* We first look up the PATH environment variable because
           we will be making a copy of it. This avoids trouble
           lateron when we will be calling getenv() again. */
        path = getenv("PATH");
        if (path == NULL)
            path = (char *) __clib4->__default_path;

        path_copy = strdup(path);
        if (path_copy == NULL) {
            __set_errno(ENOMEM);
            goto out;
        }

        path = path_copy;

        path_delimiter = getenv("PATH_SEPARATOR");
        if (path_delimiter == NULL)
            path_delimiter = __clib4->__default_path_delimiter;

        while ((search_prefix = strsep(&path, path_delimiter)) != NULL) {
            if ((*search_prefix) == '\0')
                search_prefix = ".";

            search_prefix_len = strlen(search_prefix);

            complete_path_len = search_prefix_len + 1 + command_name_len;
            if (complete_path_len + 1 > command_buffer_size) {
                char *new_command_buffer;

                /* Allocate a little more memory than we
                   really need. */
                new_command_buffer = __malloc_r(__clib4, complete_path_len + 10);
                if (new_command_buffer == NULL) {
                    __set_errno(ENOMEM);
                    goto out;
                }

                if (command_buffer != NULL)
                    free(command_buffer);

                command_buffer = new_command_buffer;
                command_buffer_size = complete_path_len + 10;
            }

            /* Combine the search prefix with the command name. */
            memcpy(command_buffer, search_prefix, search_prefix_len);
            command_buffer[search_prefix_len] = '/';
            memcpy(&command_buffer[search_prefix_len + 1], command, command_name_len);
            command_buffer[complete_path_len] = '\0';

            /* Now try to run that command. */
            result = execve(command_buffer, argv, __clib4->__environment);

            /* Did it work? And if it didn't work, did it fail because
               the command to be run could not be executed? */
            error = __get_errno();

            if (result == 0 ||
                (error != EACCES &&
                 error != EISDIR &&
                 error != ENOENT &&
                 error != ENOEXEC &&
                 error != EPERM)) {
                break;
            }

            /* Just in case somebody wants to quit... */
            __check_abort_f(__clib4);
        }
    }

out:

    if (path_copy != NULL)
        free(path_copy);

    if (command_buffer != NULL)
        free(command_buffer);

    return (result);
}
