/*
 * $Id: stdio_file_init.c,v 1.13 2008-09-04 12:07:58 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include <dos/stdio.h>

FILE_DESTRUCTOR(workbench_exit) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    /* Now clean up after the streams set up for Workbench startup... */
    if (__clib4->restore_console_task) {
        SetConsolePort(__clib4->old_console_task);
        __clib4->old_console_task = NULL;
        __clib4->restore_console_task = FALSE;
    }

    if (__clib4->restore_streams) {
        SelectInput(__clib4->old_input);
        __clib4->old_input = BZERO;

        SelectOutput(__clib4->old_output);
        __clib4->old_output = BZERO;

        SelectErrorOutput(__clib4->old_error);
        __clib4->old_error = BZERO;

        __clib4->restore_streams = FALSE;
    }

    if (__clib4->input != BZERO) {
        Close(__clib4->input);
        __clib4->input = BZERO;
    }

    if (__clib4->output != BZERO) {
        Close(__clib4->output);
        __clib4->output = BZERO;
    }

    if (__clib4->error != BZERO) {
        Close(__clib4->error);
        __clib4->error = BZERO;
    }

    LEAVE();
}

static int
wb_file_init(struct _clib4 *__clib4) {
    int result = ERROR;

    __clib4->__original_current_directory = SetCurrentDir(__clib4->__WBenchMsg->sm_ArgList[0].wa_Lock);
    __clib4->__current_directory_changed = TRUE;

    if (__clib4->__WBenchMsg->sm_ToolWindow != NULL) {
        __clib4->input = Open(__clib4->__WBenchMsg->sm_ToolWindow, MODE_NEWFILE);
    } else {
        static const char console_prefix[] = "CON:20/20/600/150/";
        static const char console_suffix[] = " Output/AUTO/CLOSE/WAIT";
        STRPTR window_specifier;
        STRPTR tool_name;
        size_t len;

        tool_name = (STRPTR) FilePart(__clib4->__WBenchMsg->sm_ArgList[0].wa_Name);

        len = strlen(console_prefix) + strlen(tool_name) + strlen(console_suffix);

        window_specifier = AllocVecTags(len + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_DONE);
        if (window_specifier == NULL)
            goto out;

        strcpy(window_specifier, console_prefix);
        strcat(window_specifier, tool_name);
        strcat(window_specifier, console_suffix);

        __clib4->input = Open(window_specifier, MODE_NEWFILE);

        FreeVec(window_specifier);
    }

    if (__clib4->input == BZERO)
        __clib4->input = Open("NIL:", MODE_NEWFILE);

    if (__clib4->input != BZERO) {
        struct FileHandle *fh = BADDR(__clib4->input);

        __clib4->old_console_task = SetConsolePort(fh->fh_MsgPort);

        __clib4->output = Open("CONSOLE:", MODE_NEWFILE);
        if (__clib4->output != BZERO)
            __clib4->restore_console_task = TRUE;
        else
            SetConsolePort(__clib4->old_console_task);

        __clib4->error = Open("CONSOLE:", MODE_NEWFILE);
        if (__clib4->error != BZERO)
            __clib4->restore_console_task = TRUE;
        else
            SetConsolePort(__clib4->old_console_task);
    }

    if (__clib4->output == BZERO)
        __clib4->output = Open("NIL:", MODE_NEWFILE);

    if (__clib4->input == BZERO || __clib4->output == BZERO || __clib4->error == BZERO)
        goto out;

    __clib4->old_input = SelectInput(__clib4->input);
    __clib4->old_output = SelectOutput(__clib4->output);
    __clib4->old_error = SelectErrorOutput(__clib4->error);
    __clib4->restore_streams = TRUE;

    result = OK;

out:

    return (result);
}

FILE_CONSTRUCTOR(stdio_file_init) {
    struct SignalSemaphore *stdio_lock;
    struct SignalSemaphore *fd_lock;
    BPTR default_file;
    ULONG fd_flags, iob_flags;
    BOOL success = FALSE;
    char *buffer;
    char *aligned_buffer;
    int i;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    uint32 physical_alignment = 0;

    GetCPUInfoTags(GCIT_CacheLineSize, &physical_alignment, TAG_DONE);
    SHOWVALUE(physical_alignment);
    if (__clib4->__cache_line_size < physical_alignment) {
        __clib4->__cache_line_size = physical_alignment;
    }

    /* If we were invoked from Workbench, set up the standard I/O streams. */
    if (__clib4->__WBenchMsg != NULL) {
        SHOWMSG("set up the standard I/O streams");
        if (wb_file_init(__clib4) < 0) {
            goto out;
        }
    }

    SHOWMSG("Now initialize the standard I/O streams (input, output, error)");
    /* Now initialize the standard I/O streams (input, output, error). */
    for (i = STDIN_FILENO; i <= STDERR_FILENO; i++) {
        switch (i) {
            case STDIN_FILENO:

                iob_flags = IOBF_IN_USE | IOBF_READ | IOBF_NO_NUL | IOBF_BUFFER_MODE_LINE;
                fd_flags = FDF_IN_USE | FDF_READ | FDF_NO_CLOSE | FDF_IS_INTERACTIVE | FDF_STDIO;
                default_file = Input();
                break;

            case STDOUT_FILENO:

                iob_flags = IOBF_IN_USE | IOBF_WRITE | IOBF_NO_NUL | IOBF_BUFFER_MODE_LINE;
                fd_flags = FDF_IN_USE | FDF_WRITE | FDF_NO_CLOSE | FDF_IS_INTERACTIVE | FDF_STDIO;
                default_file = Output();
                break;

            case STDERR_FILENO:

                iob_flags = IOBF_IN_USE | IOBF_WRITE | IOBF_NO_NUL | IOBF_BUFFER_MODE_LINE;
                fd_flags = FDF_IN_USE | FDF_WRITE | FDF_NO_CLOSE | FDF_IS_INTERACTIVE | FDF_STDIO;
                default_file = ErrorOutput();
                break;
        }

        /* Allocate a little more memory than necessary and align the buffer to a cache line boundary. */
        buffer = AllocVecTags(BUFSIZ + (__clib4->__cache_line_size - 1),
                              AVT_Type, MEMF_SHARED,
                              AVT_Alignment, __clib4->__cache_line_size,
                              AVT_ClearWithValue, 0,
                              TAG_DONE);
        if (buffer == NULL)
            goto out;

        /* Allocate memory for an arbitration mechanism, then initialize it. */
        stdio_lock = __create_semaphore();
        fd_lock = __create_semaphore();

        if (stdio_lock == NULL || fd_lock == NULL) {
            __delete_semaphore(stdio_lock);
            __delete_semaphore(fd_lock);
            goto out;
        }

        /* We ignore the file handle and let the file I/O code in the fd hook pick up the appropriate
            Input/Output/ErrorOutput handle. */
        default_file = i;

        D(("File %ld", i));
        __initialize_fd(__clib4->__fd[i], __fd_hook_entry, default_file, fd_flags, fd_lock);
        __initialize_iob(__clib4->__iob[i],
                         __iob_hook_entry,
                         buffer,
                         buffer,
                         (int64_t) BUFSIZ,
                         i,
                         i,
                         iob_flags,
                         stdio_lock);
        SHOWPOINTER(__clib4->__iob[i]);
    }

    success = TRUE;

out:

    SHOWVALUE(success);
    LEAVE();

    if (success)
        CONSTRUCTOR_SUCCEED();
    else
        CONSTRUCTOR_FAIL();
}
