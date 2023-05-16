/*
 * $Id: stdio_file_init.c,v 1.13 2008-09-04 12:07:58 clib2devs Exp $
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
    struct _clib2 *__clib2 = __CLIB2;

    /* Now clean up after the streams set up for Workbench startup... */
    if (__clib2->restore_console_task) {
        SetConsoleTask((struct MsgPort *) __clib2->old_console_task);
        __clib2->old_console_task = NULL;
        __clib2->restore_console_task = FALSE;
    }

    if (__clib2->restore_streams) {
        SelectInput(__clib2->old_input);
        __clib2->old_input = ZERO;

        SelectOutput(__clib2->old_output);
        __clib2->old_output = ZERO;

        __clib2->restore_streams = FALSE;
    }

    if (__clib2->input != ZERO) {
        SetMode(__clib2->input, DOSFALSE);

        Close(__clib2->input);
        __clib2->input = ZERO;
    }

    if (__clib2->output != ZERO) {
        SetMode(__clib2->output, DOSFALSE);

        Close(__clib2->output);
        __clib2->output = ZERO;
    }

    LEAVE();
}

static int
wb_file_init(struct _clib2 *__clib2) {
    int result = ERROR;

    __clib2->__original_current_directory = CurrentDir(__clib2->__WBenchMsg->sm_ArgList[0].wa_Lock);
    __clib2->__current_directory_changed = TRUE;

    if (__clib2->__stdio_window_specification != NULL) {
        __clib2->input = Open(__clib2->__stdio_window_specification, MODE_NEWFILE);
    } else if (__clib2->__WBenchMsg->sm_ToolWindow != NULL) {
        __clib2->input = Open(__clib2->__WBenchMsg->sm_ToolWindow, MODE_NEWFILE);
    } else {
        static const char console_prefix[] = "CON:20/20/600/150/";
        static const char console_suffix[] = " Output/AUTO/CLOSE/WAIT";
        STRPTR window_specifier;
        STRPTR tool_name;
        size_t len;

        tool_name = (STRPTR) FilePart(__clib2->__WBenchMsg->sm_ArgList[0].wa_Name);

        len = strlen(console_prefix) + strlen(tool_name) + strlen(console_suffix);

        window_specifier = malloc(len + 1);
        if (window_specifier == NULL)
            goto out;

        strcpy(window_specifier, console_prefix);
        strcat(window_specifier, tool_name);
        strcat(window_specifier, console_suffix);

        __clib2->input = Open(window_specifier, MODE_NEWFILE);

        free(window_specifier);
    }

    if (__clib2->input == ZERO)
        __clib2->input = Open("NIL:", MODE_NEWFILE);

    if (__clib2->input != ZERO) {
        struct FileHandle *fh = BADDR(__clib2->input);

        __clib2->old_console_task = SetConsoleTask(fh->fh_Type);

        __clib2->output = Open("CONSOLE:", MODE_NEWFILE);
        if (__clib2->output != ZERO)
            __clib2->restore_console_task = TRUE;
        else
            SetConsoleTask((struct MsgPort *) __clib2->old_console_task);
    }

    if (__clib2->output == ZERO)
        __clib2->output = Open("NIL:", MODE_NEWFILE);

    if (__clib2->input == ZERO || __clib2->output == ZERO)
        goto out;

    __clib2->old_input = SelectInput(__clib2->input);
    __clib2->old_output = SelectOutput(__clib2->output);

    __clib2->restore_streams = TRUE;

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
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    uint32 physical_alignment = 0;

    GetCPUInfoTags(GCIT_CacheLineSize, &physical_alignment, TAG_DONE);
    SHOWVALUE(physical_alignment);
    if (__clib2->__cache_line_size < physical_alignment) {
        __clib2->__cache_line_size = physical_alignment;
    }

    /* If we were invoked from Workbench, set up the standard I/O streams. */
    if (__clib2->__WBenchMsg != NULL) {
        SHOWMSG("set up the standard I/O streams");
        if (wb_file_init(__clib2) < 0) {
            goto out;
        }
    }

    SHOWMSG("Now initialize the standard I/O streams (input, output, error)");
    /* Now initialize the standard I/O streams (input, output, error). */
    for (i = STDIN_FILENO; i <= STDERR_FILENO; i++) {
        switch (i) {
            case STDIN_FILENO:

                iob_flags = IOBF_IN_USE | IOBF_READ | IOBF_NO_NUL | IOBF_BUFFER_MODE_LINE;
                fd_flags = FDF_IN_USE | FDF_READ | FDF_NO_CLOSE | FDF_IS_INTERACTIVE;
                default_file = Input();
                break;

            case STDOUT_FILENO:

                iob_flags = IOBF_IN_USE | IOBF_WRITE | IOBF_NO_NUL | IOBF_BUFFER_MODE_LINE;
                fd_flags = FDF_IN_USE | FDF_WRITE | FDF_NO_CLOSE | FDF_IS_INTERACTIVE;
                default_file = Output();
                break;

            case STDERR_FILENO:
            default:

                iob_flags = IOBF_IN_USE | IOBF_WRITE | IOBF_NO_NUL | IOBF_BUFFER_MODE_NONE;
                fd_flags = FDF_IN_USE | FDF_WRITE;
                default_file = ZERO;
                break;
        }

        /* Allocate a little more memory than necessary. */
        buffer = AllocVecTags(BUFSIZ + (__clib2->__cache_line_size - 1), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
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

        fd_flags |= FDF_NO_CLOSE | FDF_STDIO;

        /* Align the buffer start address to a cache line boundary. */
        aligned_buffer = (char *) ((ULONG)(buffer + (__clib2->__cache_line_size - 1)) & ~(__clib2->__cache_line_size - 1));
        D(("File %ld", i));
        __initialize_fd(__clib2->__fd[i], __fd_hook_entry, default_file, fd_flags, fd_lock);
        __initialize_iob(__clib2->__iob[i],
                         __iob_hook_entry,
                         buffer,
                         aligned_buffer,
                         (int64_t) BUFSIZ,
                         i,
                         i,
                         iob_flags,
                         stdio_lock);
        SHOWPOINTER(__clib2->__iob[i]);
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
