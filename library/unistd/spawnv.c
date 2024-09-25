/*
 * $Id: unistd_spawnv.c,v 1.0 2021-01-21 18:26:47 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include "children.h"

STATIC BOOL
string_needs_quoting(const char *string, size_t len) {
    BOOL result = FALSE;
    size_t i;
    char c;

    for (i = 0; i < len; i++) {
        c = (*string++);
        if (c == ' ' || ((unsigned char) c) == 0xA0 || c == '\t' || c == '\n' || c == '\"') {
            result = TRUE;
            break;
        }
    }

    return (result);
}

STATIC void
build_arg_string(char *const argv[], char *arg_string) {
    BOOL first_char = TRUE;
    size_t i, j, len;
    char *s;

    /* The first argv[] element is skipped; it does not contain part of
	   the command line but holds the name of the program to be run. */
    for (i = 1; argv[i] != NULL; i++) {
        s = (char *) argv[i];

        len = strlen(s);
        if (len > 0) {
            if (first_char)
                first_char = FALSE;
            else
                (*arg_string++) = ' ';

            if ((*s) != '\"' && string_needs_quoting(s, len)) {
                (*arg_string++) = '\"';

                for (j = 0; j < len; j++) {
                    if (s[j] == '\"' || s[j] == '*') {
                        (*arg_string++) = '*';
                        (*arg_string++) = s[j];
                    } else if (s[j] == '\n') {
                        (*arg_string++) = '*';
                        (*arg_string++) = 'N';
                    } else {
                        (*arg_string++) = s[j];
                    }
                }

                (*arg_string++) = '\"';
            } else {
                memcpy(arg_string, s, len);
                arg_string += len;
            }
        }
    }
}

STATIC size_t
count_extra_escape_chars(const char *string, size_t len) {
    size_t count = 0;
    size_t i;
    char c;

    for (i = 0; i < len; i++) {
        c = (*string++);
        if (c == '\"' || c == '*' || c == '\n')
            count++;
    }

    return (count);
}

STATIC size_t
get_arg_string_length(char *const argv[]) {
    size_t result = 0;
    size_t i, len = 0;
    char *s;

    /* The first argv[] element is skipped; it does not contain part of
	   the command line but holds the name of the program to be run. */
    for (i = 1; argv[i] != NULL; i++) {
        s = (char *) argv[i];

        len = strlen(s);
        if (len > 0) {
            if ((*s) != '\"') {
                if (string_needs_quoting(s, len))
                    len += 1 + count_extra_escape_chars(s, len) + 1;
            }

            if (result == 0)
                result = len;
            else
                result = result + 1 + len;
        }
    }

    return (result);
}

int
spawnv(int mode, const char *file, const char **argv) {
    int ret = -1;
    char *arg_string = NULL;
    size_t arg_string_len = 0;
    size_t parameter_string_len = 0;
    struct name_translation_info path_nti;
    struct _clib4 *__clib4 = __CLIB4;

    if (mode != P_WAIT && mode != P_NOWAIT) {
        __set_errno(ENOSYS);
        return ret;
    }

    __set_errno(0);

    if (__clib4->__unix_path_semantics) {
        if (__translate_unix_to_amiga_path_name(&file, &path_nti) != 0) {
            __set_errno(EINVAL);
            return ret;
        }
    }

    parameter_string_len = get_arg_string_length((char *const *) argv);
    // if (parameter_string_len > _POSIX_ARG_MAX) {
    //     __set_errno(E2BIG);
    //     return ret;
    // }

    arg_string = __malloc_r(__clib4, parameter_string_len + 1);
    if (arg_string == NULL) {
        __set_errno(ENOMEM);
        return ret;
    }

    if (parameter_string_len > 0) {
        build_arg_string((char *const *) argv, &arg_string[arg_string_len]);
        arg_string_len += parameter_string_len;
    }

    /* Add a NUL, to be nice... */
    arg_string[arg_string_len] = '\0';

    char finalpath[PATH_MAX] = {0};
    char processName[NAMELEN] = {0};
    snprintf(finalpath, PATH_MAX - 1, "%s %s", file, arg_string);
    snprintf(processName, NAMELEN - 1, "Spawned Process #%d", __clib4->__children);

    struct Process *me = __clib4->self;
    BPTR in  = mode == P_WAIT ? me->pr_CIS : 0;
    BPTR out  = mode == P_WAIT ? me->pr_COS : 0;
    D(("Launching [%s]", finalpath));
    ret = SystemTags(finalpath,
                     SYS_Input,     in,
                     SYS_Output,    out,
                     SYS_Error,     out,
                     SYS_UserShell, TRUE,
                     SYS_Asynch,    mode == P_WAIT ? FALSE : TRUE,
                     NP_EntryCode,  spawnedProcessEnter,
                     NP_ExitCode,   spawnedProcessExit,
                     NP_Name,       strdup(processName),
                     NP_Child,      TRUE,
                     TAG_DONE);
    if (ret != 0) {
        /* SystemTags failed. Clean up file handles */
        if (in != 0)
            Close(in);
        errno = __translate_io_error_to_errno(IoErr());
    }
    else {
        /*
         * If mode is set as P_NOWAIT we can retrieve process id calling IoErr()
         * just after SystemTags. In this case spawnv will return pid
         */
        if (mode == P_NOWAIT) {
            ret = IoErr(); // This is our ProcessID;
        }
    }
    return ret;
}