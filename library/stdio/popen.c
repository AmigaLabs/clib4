/*
 * $Id: stdio_popen.c,v 1.10 2008-05-07 09:33:55 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include "children.h"
#include <sys/wait.h>

int
pclose(FILE *stream) {
    int result = ERROR;

    ENTER();

    SHOWPOINTER(stream);

    assert(stream != NULL);

    __check_abort();

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno(EFAULT);
        goto out;
    }

    int status = 0;
    pid_t pid = findSpawnedChildrenPidByPipe(stream);
    SHOWVALUE(pid);
    waitpid(pid, &status, 0);

    SHOWMSG("Closing stream.");
    fclose(stream);

    /* ZZZ we actually could catch the program's termination code
     * by passing an exit function address to SystemTags() below.
     */
    if(status != -1)
        result = WEXITSTATUS(status);

out:

    RETURN(result);
    return (result);
}

FILE *
popen(const char *command, const char *type) {
    struct name_translation_info command_nti;
    char *command_copy = NULL;
    BPTR input = BZERO;
    BPTR output = BZERO;
    BPTR error = BZERO;
    char pipe_file_name[48];
    FILE *result = NULL;
    LONG status;
    unsigned long task_address;
    time_t now = 0;
    int i;

    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(command);
    SHOWSTRING(type);

    assert(command != NULL && type != NULL);

    __check_abort_f(__clib4);

    if (command == NULL || type == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    /* The first character selects the access mode: read or write. We don't
       support anything else. */
    switch (type[0]) {
        case 'r':
            SHOWMSG("read mode");
            break;
        case 'w':
            SHOWMSG("write mode");
            break;
        default:
            D(("unsupported access mode '%lc'", type[0]));
            __set_errno(EINVAL);
            goto out;
    }

    /* The current PIPE: device only supports unidirectional connections. Worse: even if
       a PIPE: device with bidirectional connection support were available, we would
       be unable to detect this property. */
    if ((type[1] == '+') || (type[1] != '\0' && type[2] == '+')) {
        D(("unsupported access mode '%s'", type));

        __set_errno(EINVAL);
        goto out;
    }

    if (__clib4->__unix_path_semantics) {
        char just_the_command_name[MAXPATHLEN + 1];
        BOOL quotes_needed = FALSE;
        char *command_name;
        size_t command_len;
        BOOL have_quote;
        size_t len;

        /* We may need to replace the path specified for the command,
            so let's figure out first how long the command name,
            including everything, really is. */
        len = strlen(command);
        command_len = len;

        have_quote = FALSE;
        for (i = 0; i < (int) len; i++) {
            if (command[i] == '\"') {
                quotes_needed = TRUE;

                have_quote ^= TRUE;
            }

            if ((command[i] == ' ' || command[i] == '\t') && NOT have_quote) {
                command_len = i;
                break;
            }
        }

        /* This may be too long for proper translation... */
        if (command_len > MAXPATHLEN) {
            __set_errno_r(__clib4, ENAMETOOLONG);

            result = NULL;
            goto out;
        }

        /* Grab the command name itself, then have it translated. */
        command_name = just_the_command_name;
        for (i = 0; (size_t) i < command_len; i++) {
            if (command[i] != '\"')
                (*command_name++) = command[i];
        }

        (*command_name) = '\0';

        command_name = just_the_command_name;

        if (__translate_unix_to_amiga_path_name((const char **) &command_name, &command_nti) != 0) {
            result = NULL;
            goto out;
        }

        /* Now put it all together again */
        command_copy = malloc(1 + strlen(command_name) + 1 + strlen(&command[command_len]) + 1);
        if (command_copy == NULL) {
            __set_errno_r(__clib4, ENOMEM);

            result = NULL;
            goto out;
        }

        if (quotes_needed) {
            command_copy[0] = '\"';
            strcpy(&command_copy[1], command_name);
            strcat(command_copy, "\"");
        } else {
            strcpy(command_copy, command_name);
        }

        strcat(command_copy, &command[command_len]);

        command = command_copy;
    }

    /* Build a (hopefully) unique name for the pipe stream to open. We
       construct it from the current process address, converted into
       an octal number, followed by the current time (in seconds),
       converted into another octal number. */
    strcpy(pipe_file_name, "PIPE:");

    struct Task *task = (struct Task *) __clib4->self;
    task_address = (unsigned long) task;

    for (i = strlen(pipe_file_name); task_address != 0 && i < (int) sizeof(pipe_file_name) - 1; i++) {
        pipe_file_name[i] = '0' + (task_address % 8);
        task_address = task_address / 8;
    }

    pipe_file_name[i++] = '.';

    time(&now);

    for (; now != 0 && i < (int) sizeof(pipe_file_name) - 1; i++) {
        pipe_file_name[i] = '0' + (now % 8);
        now = now / 8;
    }

    memcpy(pipe_file_name+39, "/NOBLOCK", 8);
    pipe_file_name[47] = '\0';

    SHOWSTRING(pipe_file_name);

    /* Now open the input and output streams for the program to launch. */
    if (type[0] == 'r') {
        /* Read mode: we want to read the output of the program; the program
           should read from "NIL:". */
        input = Open("NIL:", MODE_OLDFILE);
        if (input != BZERO) {
            output = Open(pipe_file_name, MODE_NEWFILE);
        }
    } else {
        /* Write mode: we want to send data to the program; the program
           should write to "NIL:". */
        input = Open(pipe_file_name, MODE_NEWFILE);
        if (input != BZERO)
            output = Open("NIL:", MODE_OLDFILE);
    }
    // error = DupFileHandle(ErrorOutput()); //
    error = Open("NIL:", MODE_OLDFILE);

    /* Check if both I/O streams could be opened. */
    if (input == BZERO || output == BZERO || error == BZERO) {
        SHOWMSG("couldn't open the streams");

        __set_errno_r(__clib4, __translate_io_error_to_errno(IoErr()));
        goto out;
    }

    D(("Launching [%s]", command));

    int asynch = TRUE; //FALSE

    /* Now try to launch the program. */
    status = SystemTags((STRPTR) command,
                        SYS_Input,          input,
                        SYS_Output,         output,
                        SYS_Error,          error, // <-- We have to block this with NIL:
                        NP_CloseError,      TRUE,
                        SYS_Asynch,         asynch,
                        SYS_UserShell,      TRUE,
                        NP_StackSize,       2024*1024,
                        NP_Name,            command,
                        NP_EntryCode,       spawnedProcessEnter,
                        NP_EntryData,       getgid(),
                        NP_ExitCode,        spawnedProcessExit,
                        NP_Child,           TRUE,
                        TAG_END);

    uint32 ret;

    /* If SYS_Asynch is FALSE, we need these. */

    if (asynch == FALSE) {
        Close(input);
        Close(output);
        // Close(error); ??
    }

    /* If launching the program returned -1 then it could not be started.
       We'll need to close the I/O streams we opened above. */

    if (status == -1) {
        SHOWMSG("SystemTagList() failed");

        __set_errno_r(__clib4, __translate_io_error_to_errno(IoErr()));
        goto out;
    } else {
        SHOWMSG("SystemTags() call completed.");
        /*
         * If mode is set as P_NOWAIT we can retrieve process id calling IoErr()
         * just after SystemTags. In this case spawnv will return pid
         */
        ret = IoErr(); // This is our ProcessID;
    }
    /* OK, the program is running. Once it terminates, it will automatically
       shut down the streams we opened for it. */
    input = output = error = BZERO;

    /* Now try to open the pipe we will use to exchange data with the program. */
    result = fopen(pipe_file_name, type);

    if(result) {
        /* We need to mark this as a pipe */
        struct iob *file = (struct iob *) result;
        int f = file->iob_Descriptor;
        struct fd *fd = __get_file_descriptor(__clib4, f);
        fd->fd_Flags |= FDF_PIPE;

        /* Add this pipe to the childrens list */
        addSpawnedChildrenPipeHandle(ret, result); //pid, pipe
    }

out:

    if (command_copy != NULL)
        free(command_copy);

    if (input != BZERO)
        Close(input);

    if (output != BZERO)
        Close(output);

    if (error != BZERO)
        Close(error);

    RETURN(result);
    return (result);
}
