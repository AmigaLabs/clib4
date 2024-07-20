/*
 * $Id: stdlib_system.c,v 1.10 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

int
system(const char *command) {
    char *command_copy = NULL;
    int result;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    if (command == NULL)
        SHOWPOINTER(command);
    else
        SHOWSTRING(command);

    __check_abort_f(__clib4);

    /* A NULL pointer for the name of the command to execute is
     * really a query to find out whether a shell is available.
     * We return 1 (TRUE).
     */
    if (command == NULL) {
        result = 1;
    } else {
        static const struct TagItem system_tags[2] =
        {
            {SYS_UserShell, TRUE},
            {TAG_END,       0}
        };

        struct name_translation_info command_nti;

        if (__clib4->__unix_path_semantics) {
            char just_the_command_name[MAXPATHLEN + 1];
            BOOL need_quotes = FALSE;
            char *command_name;
            size_t command_len;
            BOOL have_quote;
            size_t len;
            size_t i;

            /* We may need to replace the path specified for the command,
                so let's figure out first how long the command name,
                including everything, really is. */
            len = strlen(command);
            command_len = len;

            have_quote = FALSE;
            for (i = 0; i < len; i++) {
                if (command[i] == '\"') {
                    need_quotes = TRUE;
                    have_quote ^= TRUE;
                }

                if ((command[i] == ' ' || command[i] == '\t') && NOT have_quote)
                {
                    command_len = i;
                    break;
                }
            }

            /* This may be too long for proper translation... */
            if (command_len > MAXPATHLEN) {
                __set_errno(ENAMETOOLONG);

                result = ERROR;
                goto out;
            }

            /* Grab the command name itself, then have it translated. */
            command_name = just_the_command_name;
            for (i = 0; i < command_len; i++) {
                if (command[i] != '\"')
                    (*command_name++) = command[i];
            }

            (*command_name) = '\0';

            command_name = just_the_command_name;

            /* Don't try to translate the name of the command unless it has
                path name separator characters in it. */
            if (strchr(command_name, '/') != NULL &&
                __translate_unix_to_amiga_path_name((const char **) &command_name, &command_nti) != 0) {
                result = ERROR;
                goto out;
            }

            /* Now put it all together again */
            command_copy = __malloc_r(__clib4, 1 + strlen(command_name) + 1 + strlen(&command[command_len]) + 1);
            if (command_copy == NULL) {
                __set_errno(ENOMEM);

                result = ERROR;
                goto out;
            }

            if (need_quotes) {
                command_copy[0] = '\"';

                strcpy(&command_copy[1], command_name);
                strcat(command_copy, "\"");
            } else {
                strcpy(command_copy, command_name);
            }

            strcat(command_copy, &command[command_len]);

            command = command_copy;
        }

        SHOWSTRING(command);

        /* Push all currently buffered output towards the file handles,
           in case the program to be launched writes to these files
           or the console, too. */
        __flush_all_files(__clib4, -1);

        result = SystemTagList((STRPTR) command, (struct TagItem *) system_tags);
    }

out:

    if (command_copy != NULL)
        __free_r(__clib4, command_copy);

    RETURN(result);
    return (result);
}
