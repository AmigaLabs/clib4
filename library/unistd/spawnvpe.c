/*
 * $Id: unistd_spawnvpe.c,v 1.0 2024-06-20 18:26:47 clib4devs Exp $
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
spawnvpe(const char *file, const char **argv, char **deltaenv, const char *dir, int fhin, int fhout, int fherr) {
    int ret = -1;
    char *arg_string = NULL;
    size_t arg_string_len = 0;
    size_t parameter_string_len = 0;
    struct fd *fd;
    D(("spawnvpe = fhin = %ld - fhout = %ld - fherr = %ld\n", fhin, fhout, fherr));
    errno = 0;

    parameter_string_len = get_arg_string_length((char *const *) argv);
    if (parameter_string_len > _POSIX_ARG_MAX) {
        __set_errno(E2BIG);
        return ret;
    }

    arg_string = malloc(parameter_string_len + 1);
    if (arg_string == NULL) {
        __set_errno(ENOMEM);
        return ret;
    }

    if (fhin >= 0) {
        fd = __get_file_descriptor(fhin);
        if (fd == NULL) {
            __set_errno(EBADF);
            return ret;
        }
    }

    if (fhout >= 0) {
        fd = __get_file_descriptor(fhout);
        if (fd == NULL) {
            __set_errno(EBADF);
            return ret;
        }
    }

    if (fherr >= 0) {
        fd = __get_file_descriptor(fherr);
        if (fd == NULL) {
            __set_errno(EBADF);
            return ret;
        }
    }

    if (parameter_string_len > 0) {
        build_arg_string((char *const *) argv, &arg_string[arg_string_len]);
        arg_string_len += parameter_string_len;
    }

    /* Add a NUL, to be nice... */
    arg_string[arg_string_len] = '\0';

    char finalpath[PATH_MAX] = {0};
    snprintf(finalpath, PATH_MAX - 1, "%s %s", file, arg_string);

    BPTR in = BZERO, out = BZERO, err = BZERO;
    int in_found = -1, out_found = -1, err_found = -1;

    if (fhin >= 0) {
        in_found = __get_default_file(fhin, &in);
        D(("1) fhin = %ld - in_found = %ld - in = %p", fhin, in_found, in));
        if (in_found == OK && fhin == STDIN_FILENO) {
            ChangeMode(CHANGE_FH, in,  CHANGE_MODE_SHARED);
            in = DupFileHandle(in);
            D(("2) fhin = %ld - in_found = %ld - in = %p", fhin, in_found, in));
        }
    }
    if (fhout >= 0) {
        out_found = __get_default_file(fhout, &out);
        D(("1) fhout = %ld - out_found = %ld - out = %p", fhout, out_found, out));
        if (out_found == OK && fhout == STDOUT_FILENO) {
            ChangeMode(CHANGE_FH, out,  CHANGE_MODE_SHARED);
            out = DupFileHandle(out);
            D(("2) fhout = %ld - out_found = %ld - out = %p", fhout, out_found, out));
        }
    }
    if (fherr >= 0) {
        err_found = __get_default_file(fherr, &err);
        D(("1) fherr = %ld - err_found = %ld - err = %p", fherr, err_found, err));
        if (err_found == OK && fherr == STDERR_FILENO) {
            ChangeMode(CHANGE_FH, err,  CHANGE_MODE_SHARED);
            err = DupFileHandle(err);
            D(("2) fherr = %ld - err_found = %ld - err = %p", fherr, err_found, err));
        }
    }

    //Printf("finalpath = %s - in_found = %ld - out_found = %ld - err_found = %ld\n", finalpath, in_found, out_found, err_found);
    ret = SystemTags(finalpath,
                       SYS_Input, in_found == OK ? in : 0,
                       SYS_Output, out_found == OK ? out : 0,
                       SYS_Error, err_found == OK ? err : 0,
                       SYS_UserShell, TRUE,
                       SYS_Asynch, TRUE,
                       NP_Child, TRUE,
                       //NP_CurrentDir, dir != NULL ? Lock(dir, SHARED_LOCK) : 0,
                       NP_Name, argv[0],
                       TAG_DONE);

    if (ret != 0) {
        Printf("Error executing %s\n", finalpath);
        /* SystemTags failed. Clean up file handles */
        if (in_found == 0) Close(in);
        if (out_found == 0) Close(out);
        if (err_found == 0) Close(err);
        errno = __translate_io_error_to_errno(IoErr());
    }
    else {
        /*
         * If mode is set as P_NOWAIT we can retrieve process id calling IoErr()
         * just after SystemTags. In this case spawnv will return pid
         */
        ret = IoErr();
        if (insertSpawnedChildren(ret, getgid())) {
            D(("Children with pid %ld and gid %ld inserted into list\n", ret, getgid()));
        }
        else {
            D(("Cannot insert children with pid %ld and gid %ld into list\n", ret, getgid()));
        }
    }
    return ret;
}