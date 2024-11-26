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
/* * * * *
    Apparently this doesn't work. You cannot delay a child by using signals in the EntryCode,
    because - as it seems - SystemTags only returns AFTER executing EntryCode. Which, of course,
    means, that the parent is never allowed to send the signal. 
 * * * * */
// struct EntryData {
//     uint8 childSignal;
//     uint8 parentSignal;
//     struct Task *parent, *child;
// };
// void amiga_entryCode(int32 entry_data);
// void
// amiga_entryCode(int32 entry_data) {
//     struct EntryData *ed = (struct EntryData *)entry_data;
//     ed->childSignal = AllocSignal(-1);
//     ed->child = FindTask(0);
//     DebugPrintF("[child :] Signalling parent...\n");
//     Signal(ed->parent, 1 << ed->parentSignal);
//     DebugPrintF("[child :] Waiting for signal from parent...\n");
//     Wait(1 << ed->childSignal);
//     DebugPrintF("[child :] Done.");
//     FreeSignal(ed->childSignal);
// }
int
spawnvpe(
    const char *file,
    const char **argv,
    char **deltaenv,
    const char *_cwd,
    int fhin,
    int fhout,
    int fherr
) {
    int ret = -1;
    struct name_translation_info nti_name;
    const char *name = file;
    struct name_translation_info nti_cwd;
    const char *cwd = _cwd;
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
    BPTR seglist;

    __set_errno(0);

    D(("Starting new process [%s]\n", name));
    // printf("[spawnvpe :] Starting new process [%s]\n", name);

    int error = __translate_unix_to_amiga_path_name(&name, &nti_name);
    if (error) {
        __set_errno(EINVAL);
        D(("__translate_unix_to_amiga_path_name failed: %s\n", strerror(error)));
        return ret;
    }

    D(("name after conversion: [%s]\n", name));
    // printf("[spawnvpe :] name after conversion [%s]\n", name);

#if 0
    seglist = LoadSeg(name);
	if (!seglist)
		return -1;
#endif

    BPTR fileLock = Lock(name, SHARED_LOCK);
    if (fileLock) {
        progdirLock = ParentDir(fileLock);
        UnLock(fileLock);
    }

    // printf("[spawnvpe :] cwd before conversion: [%s]\n", cwd);

    if(cwd) {
        error = __translate_unix_to_amiga_path_name(&cwd, &nti_cwd);
        if (error) {
            __set_errno(EINVAL);
            D(("__translate_unix_to_amiga_path_name failed: %s\n", strerror(error)));
            return ret;
        }
    }

    D(("cwd after conversion: [%s]\n", cwd));
    // printf("[spawnvpe :] cwd after conversion [%s]\n", cwd);

    BPTR cwdLock = cwd ? Lock(cwd, SHARED_LOCK) : 0;

    parameter_string_len = get_arg_string_length((char *const *) argv);
    // This is probably unnecessary :
    //
    // if (parameter_string_len > _POSIX_ARG_MAX) {
    //     __set_errno(E2BIG);
    //     return ret;
    // }

    D(("parameter_string_len: [%ld]\n", parameter_string_len));
    // printf("[spawnvpe :] parameter_string_len: [%ld]\n", parameter_string_len);

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

    // printf("[spawnvpe :] full command == <%s>\n", finalpath);
    // printf("[spawnvpe :] processName == <%s>\n", processName);

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

    // struct EntryData ed;
    // ed.parent = FindTask(0);
    // ed.parentSignal = AllocSignal(-1);

    struct Task *_me = FindTask(0);
#if 0
  struct Process *p = CreateNewProcTags(
    NP_Seglist,		seglist,
    NP_FreeSeglist,	TRUE,

    NP_Cli,			TRUE,
    NP_Child,		TRUE,
    NP_NotifyOnDeathSigTask, _me,

#if 1
    NP_Input,		iofh[0],
    NP_CloseInput,	closefh[0],
    NP_Output,		iofh[1],
    NP_CloseOutput,	closefh[1],
    NP_Error,		iofh[2],
    NP_CloseError,	closefh[2],
#else
    NP_Input,		IDOS->Input(),
    NP_CloseInput,	FALSE,
    NP_Output,		IDOS->Output(),
    NP_CloseOutput,	FALSE,
    NP_Error,		IDOS->ErrorOutput(),
    NP_CloseError,	FALSE,
#endif

    NP_EntryCode, spawnvpe_entryCode,
    NP_EntryData, &ed,

    // NP_FinalCode,	amiga_FinalCode,
    // NP_FinalData,	fd,

    // NP_EntryCode,  spawnedProcessEnter,
    NP_ExitCode,   spawnedProcessExit,

    NP_Name,      strdup(processName),
    cwdLock ? NP_CurrentDir : TAG_SKIP, cwdLock,
    progdirLock ? NP_ProgramDir : TAG_SKIP, progdirLock,
    NP_Arguments, arg_string,
    TAG_DONE
  );
  if (p) ret = 0;
#else
    // printf("(*)Calling SystemTags.\n");

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

                     NP_EntryCode,  spawnedProcessEnter,
                     NP_EntryData, getgid(),
                    //  NP_EntryCode,  amiga_entryCode,
                    //  NP_EntryData,  &ed,
                     NP_ExitCode,   spawnedProcessExit,

                     TAG_DONE);

#endif

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
         * IoErr() must be called IMMEDIATELY after SystemTags() == no other DOS calls inbetween
         */

        // DebugPrintF("[main :] Child created with success.\n");

        pid_t pid = IoErr();
        // gid_t groupId = getgid();
        // uint32 ppid = ((struct Process *) FindTask(NULL))->pr_ProcessID;

        // if (insertSpawnedChildren(pid, ppid, groupId)) {
        //     __CLIB4->__children++;
        //     D(("Children with pid %ld and gid %ld inserted into list\n", pid, groupId));
        // }
        // else {
        //     D(("Cannot insert children with pid %ld and gid %ld into list\n", pid, groupId));
        // }

        // DebugPrintF("[parent :] Waiting for signal from child...\n");
        // Wait(1 << ed.parentSignal);
        // DebugPrintF("[parent :] Signalling child...\n");
        // Signal(ed.child, 1 << ed.childSignal);
        // DebugPrintF("[parent :] Done.\n");
        // FreeSignal(ed.parentSignal);

        ret = pid;
    }
    // D(("SystemTags completed. return value: [%ld]\n", ret));
    // printf("SystemTags completed. return value: [%ld]\n", ret);

    return ret;
}
