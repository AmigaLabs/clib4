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

struct EntryData {
    uint8 signal;
    struct Task *mainTask;
};

STATIC void amiga_EntryCode(int32 entry_data) {
    // NOTES:
    // We need to make sure, that the spawned process exists, before the parent can continue.
    // See the notes for : uv__spawn_and_init_child_posix_spawn
    struct EntryData *ed = (struct EntryData *) entry_data;
    if(ed)
        Signal(ed->mainTask, 1 << ed->signal);
}

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

int spawnvpe(const char *file, const char **argv, char **deltaenv, const char *dir, int fhin, int fhout, int fherr) {
    int ret = -1;
    struct name_translation_info nti_name;
    const char *name = file;
    struct name_translation_info nti_cwd;
    BPTR iofh[3] = {BZERO, BZERO, BZERO};
    int closefh[3] = {FALSE, FALSE, FALSE};
    BPTR fh;
    int err;
    BPTR progdirLock = 0;
    struct Task *me = FindTask(NULL);
    char *arg_string = NULL;
    size_t arg_string_len = 0;
    size_t parameter_string_len = 0;

    arg_string = malloc(parameter_string_len + 1);
    if (arg_string == NULL) {
        __set_errno(ENOMEM);
        return ret;
    }

    parameter_string_len = get_arg_string_length((char *const *) argv);
    if (parameter_string_len > _POSIX_ARG_MAX) {
        __set_errno(E2BIG);
        return ret;
    }

    DebugPrintF("[uv__do_create_new_process_amiga :] New process \'%s\'\n", name);

    int error = __translate_unix_to_amiga_path_name(&name, &nti_name);
    if (error) {
        printf("%s\n", strerror(error));
        return ret;
    }

    BPTR fileLock = Lock(name, SHARED_LOCK);
    if (fileLock) {
        progdirLock = ParentDir(fileLock);
        UnLock(fileLock);
    }

    BPTR seglist = LoadSeg(name);
    if (!seglist)
        return ret;

    if (parameter_string_len > 0) {
        build_arg_string((char *const *) argv, &arg_string[arg_string_len]);
        arg_string_len += parameter_string_len;
    }

    /* Add a NUL, to be nice... */
    arg_string[arg_string_len] = '\0';

    char finalpath[PATH_MAX] = {0};
    snprintf(finalpath, PATH_MAX - 1, "%s %s", file, arg_string);

    Printf("File to execute: [%s]\n", finalpath);

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

    struct EntryData *ed = (struct EntryData *) AllocVecTags(sizeof(struct EntryData), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_DONE);

    ed->signal = AllocSignal(-1);
    ed->mainTask = me;

    struct Process *p = CreateNewProcTags(
            NP_Seglist, seglist,
            NP_FreeSeglist, TRUE,
            NP_Cli, TRUE,
            NP_Child, TRUE,
            NP_NotifyOnDeathSigTask, me,
            NP_Input, iofh[0],
            NP_CloseInput, closefh[0],
            NP_Output, iofh[1],
            NP_CloseOutput, closefh[1],
            NP_Error, iofh[2],
            NP_CloseError, closefh[2],
            NP_EntryCode, amiga_EntryCode,
            NP_EntryData, ed,
            NP_FinalCode, spawnedProcessExit,
            NP_Name, argv[0],
            progdirLock ? NP_ProgramDir : TAG_SKIP, progdirLock,
            NP_Arguments, arg_string,
            TAG_DONE
    );

    if (p == NULL) {
        FreeVec(ed);
        for (int i = 0; i < 3; i++) {
            if (closefh[i])
                Close(iofh[i]);
        }
        return ret;
    }
    ret = p->pr_ProcessID;

    // wait for the entry signal from the child :
    Wait(1 << ed->signal | SIGBREAKF_CTRL_C);
    FreeSignal(ed->signal);
    FreeVec(ed);

    // success
    return ret;
}