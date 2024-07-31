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
spawnvpe_callback(
    const char *file,
    const char **argv,
    char **deltaenv,
    const char *cwd,
    int fhin,
    int fhout,
    int fherr,

    void (*entry_fp)(void *), void* entry_data,
    void (*final_fp)(int, void *), void* final_data
) {
    int ret = -1;
    struct name_translation_info nti_name;
    const char *name = file;
    struct name_translation_info nti_cwd;
    BPTR iofh[3] = {BZERO, BZERO, BZERO};
    int closefh[3] = {FALSE, FALSE, FALSE};
    BPTR fh;
    int err;
    BPTR progdirLock = 0;
    char *arg_string = NULL;
    size_t arg_string_len = 0;
    size_t parameter_string_len = 0;
    struct _clib4 *__clib4 = __CLIB4;
    struct Task *me = (struct Task *) __clib4->self;

    __set_errno(0);

    D(("Starting new process [%s]\n", name));

    int error = __translate_unix_to_amiga_path_name(&name, &nti_name);
    if (error) {
        __set_errno(EINVAL);
        D(("__translate_unix_to_amiga_path_name failed: %s\n", strerror(error)));
        return ret;
    }

    D(("name after conversion: [%s]\n", name));

    BPTR fileLock = Lock(name, SHARED_LOCK);
    if (fileLock) {
        progdirLock = ParentDir(fileLock);
        UnLock(fileLock);
    }

    BPTR cwdLock = cwd ? Lock(cwd, SHARED_LOCK) : 0;

    parameter_string_len = get_arg_string_length((char *const *) argv);
    if (parameter_string_len > _POSIX_ARG_MAX) {
        __set_errno(E2BIG);
        return ret;
    }

    D(("parameter_string_len: [%ld]\n", parameter_string_len));

    arg_string = malloc(parameter_string_len + 1);
    if (arg_string == NULL) {
        __set_errno(ENOMEM);
        return ret;
    }

    if (parameter_string_len > 0) {
        build_arg_string((char *const *) argv, &arg_string[arg_string_len]);
        arg_string_len += parameter_string_len;
    }

    D(("arg_string_len: [%ld]\n", arg_string_len));

    /* Add a NUL, to be nice... */
    arg_string[arg_string_len] = '\0';

    D(("arg_string: [%s]\n", arg_string));

    int finalpath_len = strlen(name) + 1 + arg_string_len + 1; // '\0'
    char *finalpath = (char*)malloc(finalpath_len);
    char processName[32] = {0};
    snprintf(finalpath, finalpath_len, "%s %s", name, arg_string);
    snprintf(processName, NAMELEN - 1, "Spawned Process #%d", __clib4->__children);

    D(("File to execute: [%s]\n", finalpath));

    if (fhin >= 0) {
        err = __get_default_file(fhin, &fh);
        if (err) {
            __set_errno(EBADF);
            return ret;
        }
        iofh[0] = DupFileHandle(fh); //in case this is closed by the parent
        closefh[0] = TRUE;
    }
    else {
        iofh[0] = Open("NIL:", MODE_OLDFILE);
        closefh[0] = TRUE;
    }

    if (fhout >= 0) {
        err = __get_default_file(fhout, &fh);
        if (err) {
            __set_errno(EBADF);
            return ret;
        }
        iofh[1] = DupFileHandle(fh); //in case this is closed by the parent
        closefh[1] = TRUE;
    }
    else {
        iofh[1] = Open("NIL:", MODE_OLDFILE);
        closefh[1] = TRUE;
    }

    if (fherr >= 0) {
        err = __get_default_file(fherr, &fh);
        if (err) {
            __set_errno(EBADF);
            return ret;
        }
        iofh[2] = DupFileHandle(fh); //in case this is closed by the parent
        closefh[2] = TRUE;
    }
    else {
        iofh[2] = Open("NIL:", MODE_OLDFILE);
        closefh[2] = TRUE;
    }

    D(("(*)Calling SystemTags.\n"));

    struct Task *_me = FindTask(0);
    ret = SystemTags(finalpath,
                     NP_NotifyOnDeathSigTask, _me,
                     SYS_Input, iofh[0],
                     SYS_Output, iofh[1],
                     SYS_Error, iofh[2],
                     SYS_UserShell, TRUE,
                     SYS_Asynch, TRUE,
                     NP_Child, TRUE,
                     progdirLock ? NP_ProgramDir : TAG_SKIP, progdirLock,
                     cwdLock ? NP_CurrentDir : TAG_SKIP, cwdLock,
                     NP_Name, strdup(processName),
                     entry_fp ? NP_EntryCode    : TAG_SKIP,	entry_fp,
                     entry_data ? NP_EntryData  : TAG_SKIP, entry_data,
                     final_fp ? NP_FinalCode    : TAG_SKIP,	final_fp,
                     final_data ? NP_FinalData  : TAG_SKIP, final_data,
                     NP_ExitCode, spawnedProcessExit,
                     TAG_DONE);

    D(("SystemTags completed. return value: [%ld]\n", ret));

    if (ret != 0) {
        __set_errno(__translate_io_error_to_errno(IoErr()));
        /* SystemTags failed. Clean up file handles */
        for (int i = 0; i < 3; i++) {
            if (closefh[i])
                Close(iofh[i]);
        }
    }
    else {
        __clib4->__children++;
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
int spawnvpe(const char *file, const char **argv, char **deltaenv, const char *dir, int fhin, int fhout, int fherr) {
    return spawnvpe_callback(file, argv, deltaenv, dir, fhin, fhout, fherr, 0, 0, 0, 0);
}
