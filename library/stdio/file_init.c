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

void
workbench_exit() {
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
    	SetMode(__clib4->old_input, DOSFALSE);
        __clib4->old_input = BZERO;

        SelectOutput(__clib4->old_output);
    	SetMode(__clib4->old_output, DOSFALSE);
        __clib4->old_output = BZERO;

        SelectErrorOutput(__clib4->old_error);
    	SetMode(__clib4->old_error, DOSFALSE);
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

int
wb_file_init(struct _clib4 *__clib4) {
    int result = ERROR;
    STRPTR window_specifier = NULL;
    ENTER();

    __clib4->__original_current_directory = SetCurrentDir(__clib4->__WBenchMsg->sm_ArgList[0].wa_Lock);
    __clib4->__current_directory_changed = TRUE;

    if (__clib4->__WBenchMsg->sm_ToolWindow != NULL) {
        __clib4->input = Open(__clib4->__WBenchMsg->sm_ToolWindow, MODE_NEWFILE);
    } else {
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

        D(("window_specifier=%s\n", window_specifier));
        __clib4->input = Open(window_specifier, MODE_OLDFILE);
    }

    if (__clib4->input == BZERO) {
        SHOWMSG((" __clib4->input is BZERO. Open NIL:\n"));
        __clib4->input = Open("NIL:", MODE_OLDFILE);
    }

    if (__clib4->input != BZERO) {
        struct FileHandle *fh = BADDR(__clib4->input);

        __clib4->old_console_task = SetConsolePort(fh->fh_MsgPort);

        __clib4->output = Open("CONSOLE:", MODE_OLDFILE);
        if (__clib4->output != BZERO)
            __clib4->restore_console_task = TRUE;
        else
            SetConsolePort(__clib4->old_console_task);

        __clib4->error = Open("CONSOLE:", MODE_OLDFILE);
        if (__clib4->error != BZERO)
            __clib4->restore_console_task = TRUE;
        else
            SetConsolePort(__clib4->old_console_task);
    }

    if (__clib4->output == BZERO)
        __clib4->output = Open("NIL:", MODE_OLDFILE);

    if (__clib4->input == BZERO || __clib4->output == BZERO || __clib4->error == BZERO) {
        D(("One of stream still BZERO [%x %x %x].. Goto out\n", __clib4->input, __clib4->output, __clib4->error));
        goto out;
    }

    // Reset FDs for input/output/error
    __clib4->__fd[0]->fd_DefaultFile = __clib4->input;
    __clib4->__fd[1]->fd_DefaultFile = __clib4->output;
    __clib4->__fd[2]->fd_DefaultFile = __clib4->error;

    __clib4->old_input = SelectInput(__clib4->input);
    __clib4->old_output = SelectOutput(__clib4->output);
    __clib4->old_error = SelectErrorOutput(__clib4->error);
    __clib4->restore_streams = TRUE;

    result = OK;

out:

    if (window_specifier != NULL) {
        FreeVec(window_specifier);
        window_specifier = NULL;
    }

    RETURN(result);
    return (result);
}

FILE_CONSTRUCTOR(stdio_file_init) {
    APTR stdio_lock;
    APTR fd_lock;
    BPTR default_file;
    ULONG fd_flags, iob_flags;
    BOOL success = FALSE;
    char *buffer;
    int i;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    uint32 physical_alignment = 0;

    GetCPUInfoTags(GCIT_CacheLineSize, &physical_alignment, TAG_DONE);
    SHOWVALUE(physical_alignment);
    if (__clib4->__cache_line_size < physical_alignment) {
        __clib4->__cache_line_size = physical_alignment;
    }

    SHOWMSG("Now initialize the standard I/O streams (input, output, error)");

    /* Initialize the standard I/O streams using the static __sf[] array.
     * Also maintain backward compat: __iob[0..2] point to __sf[0..2]. */
    for (i = STDIN_FILENO; i <= STDERR_FILENO; i++) {
        switch (i) {
            case STDIN_FILENO:

                iob_flags = IOBF_IN_USE | IOBF_READ | IOBF_NO_NUL | IOBF_BUFFER_MODE_LINE;
                fd_flags = FDF_IN_USE | FDF_READ | FDF_NO_CLOSE_BPTR | FDF_STDIO;
                default_file = Input();
                break;

            case STDOUT_FILENO:

                iob_flags = IOBF_IN_USE | IOBF_WRITE | IOBF_NO_NUL | IOBF_BUFFER_MODE_LINE;
                fd_flags = FDF_IN_USE | FDF_WRITE | FDF_NO_CLOSE_BPTR | FDF_STDIO;
                default_file = Output();
                break;

            case STDERR_FILENO:

                iob_flags = IOBF_IN_USE | IOBF_WRITE | IOBF_NO_NUL | IOBF_BUFFER_MODE_NONE;
                fd_flags = FDF_IN_USE | FDF_WRITE | FDF_NO_CLOSE_BPTR | FDF_STDIO;
                default_file = ErrorOutput();
                break;
        }

		if (default_file != BZERO) {
			if (IsInteractive(default_file)) {
				SET_FLAG(fd_flags, FDF_IS_INTERACTIVE);
			}
		}

        /* Allocate a little more memory than necessary and align the buffer to a cache line boundary. */
        buffer = malloc(BUFSIZ + (__clib4->__cache_line_size - 1));
        if (buffer == NULL)
            goto out;

        /* Allocate memory for an arbitration mechanism, then initialize it. */
        stdio_lock = __create_semaphore();
        fd_lock = __create_mutex();

        if (stdio_lock == NULL || fd_lock == NULL) {
            __delete_semaphore(stdio_lock);
            __delete_mutex(fd_lock);
            goto out;
        }

        D(("File %ld", i));

        /* Initialize the fd layer (unchanged) */
        __initialize_fd(__clib4->__fd[i], __fd_hook_entry, default_file, fd_flags, fd_lock);

        /* Initialize the static __sf[i] iob struct */
        __initialize_iob(&__sf[i],
                         __iob_hook_entry,
                         buffer,
                         buffer,
                         (int64_t) BUFSIZ + (__clib4->__cache_line_size - 1),
                         i,
                         i,
                         iob_flags,
                         stdio_lock);

        /* Set up the new function pointers for I/O dispatch */
        __sf[i]._read = __sread;
        __sf[i]._write = __swrite;
        __sf[i]._seek = __sseek;
        __sf[i]._close = __sclose;
        __sf[i]._seek64 = __sseek;  /* Same implementation for 64-bit */
        __sf[i]._cookie = &__sf[i]; /* Cookie points to the iob itself */
        __sf[i]._blksize = BUFSIZ;

        /* Backward compat: make __iob[i] point to __sf[i] */
        __clib4->__iob[i] = &__sf[i];
        SHOWPOINTER(&__sf[i]);
    }

    /* Mark stdio as initialized */
    __clib4->__stdio_initialized = 1;

    success = TRUE;

out:

    SHOWVALUE(success);
    LEAVE();

    if (success)
        CONSTRUCTOR_SUCCEED();
    else
        CONSTRUCTOR_FAIL();
}
