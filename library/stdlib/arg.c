/*
 * $Id: stdlib_arg.c,v 1.16 2025-03-23 14:51:15 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static BOOL
is_space(unsigned char c) {
    BOOL result;

    result = (BOOL)(c == '\t' ||                  /* horizontal tab */
                    c == '\r' ||                  /* carriage return */
                    c == '\n' ||                  /* line feed */
                    c == '\v' ||                  /* vertical tab */
                    c == '\f' ||                  /* form feed */
                    c == ' ' ||                   /* blank space */
                    c == (unsigned char) '\240'); /* non-breaking space */

    return (result);
}

static BOOL
is_escape_character(struct _clib4 *__clib4, unsigned char c) {
    BOOL result;

    result = (BOOL)(c == '*' || c == __clib4->__shell_escape_character);

    return (result);
}

static BOOL
is_final_quote_character(const unsigned char *str) {
    BOOL result;

    result = (BOOL)(str[0] == '\"' && (str[1] == '\0' || is_space(str[1])));

    return (result);
}

BOOL
arg_init() {
    ENTER();

    BOOL success = FALSE;
    struct _clib4 *__clib4 = __CLIB4;

    /* Shell startup? */
    if (__clib4->__WBenchMsg == NULL) {
        BOOL expand_wildcard_args = FALSE;
        size_t number_of_arguments;
        const unsigned char *arg_str;
        size_t arg_len;
        unsigned char *command_line;
        unsigned char *str;

        /* Check if wildcard expansion of command line parameters
		   should be enabled. Note that the callback function, if
		   declared, takes precedence over the global variable. */
        if (__clib4->__unix_path_semantics) {
            expand_wildcard_args = __clib4->__expand_wildcard_args;

            if (FindVar("DISABLE_COMMANDLINE_WILDCARD_EXPANSION", LV_VAR) != NULL)
                expand_wildcard_args = FALSE;
        }

        /* Get the shell parameter string and find out
           how long it is, stripping a trailing line
           feed and blank spaces if necessary. */
        arg_str = (const unsigned char *) GetArgStr();
        if (arg_str == NULL) {
            /* maybe we are on a library so don't check for startup args */
            goto out;
        }

        while (is_space(*arg_str))
            arg_str++;

        arg_len = strlen((char *) arg_str);

        while (arg_len > 0 && is_space(arg_str[arg_len - 1]))
            arg_len--;

        D(("ARGLEN = %ld", arg_len));
        /* Make a copy of the shell parameter string. */
        command_line = AllocVecTags(arg_len + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_DONE);
        if (command_line == NULL)
            goto out;

        if (arg_len > 0)
            memmove(command_line, arg_str, arg_len);
        command_line[arg_len] = '\0';

        /* If we have a valid command line string and a command
           name, proceed to set up the argument vector. */
        str = command_line;

        /* Count the number of arguments. */
        number_of_arguments = 1;

        while (TRUE) {
            /* Skip leading blank space. */
            while ((*str) != '\0' && is_space(*str))
                str++;

            if ((*str) == '\0')
                break;

            number_of_arguments++;

            /* Quoted parameter starts here? */
            if ((*str) == '\"') {
                str++;

                /* Skip the quoted string. */
                while ((*str) != '\0' && NOT is_final_quote_character(str))
                {
                    /* Escape character? */
                    if (is_escape_character(__clib4, *str)) {
                        str++;

                        if ((*str) != '\0')
                            str++;
                    } else {
                        str++;
                    }
                }

                /* Skip the closing quote. */
                if ((*str) != '\0')
                    str++;
            } else {
                /* Skip the parameter. */
                while ((*str) != '\0' && NOT is_space(*str))
                str++;

                if ((*str) == '\0')
                    break;
            }
        }

        /* Put all this together into an argument vector.
           We allocate one extra slot to put a NULL pointer
           into. */
        __clib4->__argv = (char **) AllocVecTags((number_of_arguments + 1) * sizeof(*__clib4->__argv), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_DONE);
        if (__clib4->__argv == NULL)
            goto out;

        /* The first parameter is the program name. */
        __clib4->__argv[0] = __clib4->__progname;

        /* If this is using unix path semantics, we need to prep argv[0]. */
        if (__clib4->__unix_path_semantics) {
            D(("Translating argv0 [%s]\n", __clib4->__argv[0]));
            // This is unfortunately orphaned :
            struct name_translation_info *nti_argv0 = (struct name_translation_info *) AllocVecTags(sizeof(struct name_translation_info), TAG_DONE);
            /* int error = */ __translate_amiga_to_unix_path_name((char const **)&__clib4->__argv[0], nti_argv0);
            D(("Translated progname : [%s]\n", __clib4->__argv[0]));
        }

        str = command_line;

        __clib4->__argc = 1;

        while (TRUE) {
            /* Skip leading blank space. */
            while ((*str) != '\0' && is_space(*str))
                str++;

            if ((*str) == '\0')
                break;

            /* Quoted parameter starts here? */
            if ((*str) == '\"') {
                char *arg;

                if (__clib4->__unix_path_semantics) {
                    /* If necessary, indicate that this parameter was quoted. */
                    if (expand_wildcard_args && __wildcard_quote_parameter(__clib4->__argc) < 0)
                        goto out;
                }

                str++;

                __clib4->__argv[__clib4->__argc++] = (char *) str;

                arg = (char *) str;

                /* Process the quoted string. */
                while ((*str) != '\0' && NOT is_final_quote_character(str))
                {
                    if (is_escape_character(__clib4, *str)) {
                        str++;

                        switch (*str) {
                            /* "*e" == "\033" */
                            case 'E':
                            case 'e':

                                (*arg++) = '\033';
                                break;

                                /* "*n" == "\n" */
                            case 'N':
                            case 'n':

                                (*arg++) = '\n';
                                break;

                            case '\0':

                                /* NOTE: the termination is handled down below. */
                                break;

                            default:

                                (*arg++) = (*str);
                                break;
                        }

                        if ((*str) != '\0')
                            str++;
                    } else {
                        (*arg++) = (*str++);
                    }
                }

                /* Skip the terminating quote. */
                if ((*str) != '\0')
                    str++;

                /* Make sure that the quoted string is properly terminated. This
                   actually overwrites the final quote character. */
                (*arg) = '\0';
            } else {
                __clib4->__argv[__clib4->__argc++] = (char *) str;

                while ((*str) != '\0' && NOT is_space(*str))
                str++;

                if ((*str) == '\0')
                    break;

                (*str++) = '\0';
            }
        }

        assert(__clib4->__argc == (int) number_of_arguments);
        assert(str <= &command_line[arg_len]);

        __clib4->__argv[__clib4->__argc] = NULL;

        if (__clib4->__unix_path_semantics) {
            /* If necessary, expand wildcard patterns found in the command line string into file and directory names. */
            if (expand_wildcard_args && __wildcard_expand_init() < 0)
                goto out;
        }
    } else {
        /* Return a pointer to the startup message in place of the the argument vector.
         * The argument counter (what will come out as 'argc' for the main() function) will remain 0.
        */
        __clib4->__argv = (char **) __clib4->__WBenchMsg;
    }

    success = TRUE;

out:

    RETURN(success);
    return success;
}

void
arg_exit() {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->__WBenchMsg == NULL) {
        if (__clib4->__argv) {
            FreeVec(__clib4->__argv);
            __clib4->__argv = NULL;
        }
    }

    LEAVE();
}