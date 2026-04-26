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

    int command_len = strlen(file) + 1 + arg_string_len + 1; // '\0'
    char *command = (char *) malloc(command_len);
    char process_name[32] = {0};
    snprintf(command, command_len, "%s %s", file, arg_string);
    snprintf(process_name, NAMELEN - 1, "Spawned Process #%d", __clib4->__children);

    D(("command : [%s]\n", command));

    struct Process *me = __clib4->self;
    // These will be closed, so we need duplicates :
    BPTR in = DupFileHandle(Input());
    BPTR out = DupFileHandle(Output());
    BPTR err = DupFileHandle(ErrorOutput());
    D(("Launching [%s]", command));
	struct spawnData data = { getgid(), FindTask(NULL), "" };
	if (__CLIB4->uuid) strncpy(data.parentUuid, __CLIB4->uuid, UUID4_LEN);
    ret = SystemTags(command,
                     SYS_Input, in,
                     SYS_Output, out,
                     SYS_Error, err,
                     NP_CloseError, TRUE,
                     SYS_UserShell, TRUE,
                     SYS_Asynch, mode == P_WAIT ? FALSE : TRUE,
                     NP_EntryCode, spawnedProcessEnter,
                     NP_EntryData, &data,
                     NP_ExitCode, spawnedProcessExit,
                     NP_Name, process_name,
                     NP_Child, TRUE,
                     TAG_DONE);
    if (ret) {
        /* SystemTags failed. Clean up file handle */
        if (in)
            Close(in);
        if (out)
            Close(out);
        if (err)
            Close(err);
        errno = __translate_io_error_to_errno(IoErr());
    } else {
        /*
         * If mode is set as P_NOWAIT we can retrieve process id calling IoErr()
         * just after SystemTags. In this case spawnv will return pid
         */
        if (mode == P_NOWAIT) {
            ret = IoErr(); // This is our ProcessID;
        } else {
            if (in)
                Close(in);
            if (out)
                Close(out);
        }
    }
	free(command);

    return ret;
}