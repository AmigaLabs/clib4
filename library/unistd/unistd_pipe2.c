/*
 * $Id: unistd_pipe2.c,v 1.0 2020-01-14 12:35:27 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/****************************************************************************/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/****************************************************************************/

/* The following is not part of the ISO 'C' (1994) standard. */

/****************************************************************************/
static int pipenum = 0;

int pipe2(int fd[2], int flags)
{
    char pipe_name[1024] = {0};

    /* Check the supported flags.  */
    if ((flags & ~(O_NONBLOCK)) != 0)
    {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    ENTER();
    DECLARE_UTILITYBASE();

#ifdef USE_TEMPFILES
#if defined(__amigaos4__)
    snprintf(pipe_name, sizeof(pipe_name), "T:%x.%08x", pipenum++, ((struct Process *)FindTask(NULL))->pr_ProcessID);
#else
    snprintf(pipe_name, sizeof(pipe_name), "T:%x.%08x", pipenum++, GetUniqueID());
#endif
#else
#if defined(__amigaos4__)
    if (flags & O_NONBLOCK)
        snprintf(pipe_name, sizeof(pipe_name), "PIPE:%x%lu/32768/0/NOBLOCK", pipenum++, ((struct Process *)FindTask(NULL))->pr_ProcessID);
    else
        snprintf(pipe_name, sizeof(pipe_name), "PIPE:%x%lu/32768/0", pipenum++, ((struct Process *)FindTask(NULL))->pr_ProcessID);
#else
    if (flags & O_NONBLOCK)
        snprintf(pipe_name, sizeof(pipe_name), "PIPE:%x%08x/4096/0/NOBLOCK", pipenum++, GetUniqueID());
    else
        snprintf(pipe_name, sizeof(pipe_name), "PIPE:%x%08x/4096/0", pipenum++, GetUniqueID());
#endif
#endif // USE_TEMPFILES

    fd[1] = open(pipe_name, O_WRONLY | O_CREAT);
    fd[0] = open(pipe_name, O_RDONLY);

    if (fd[0] == -1 || fd[1] == -1)
    {
        if (fd[0] != -1)
            close(fd[0]);
        if (fd[1] != -1)
            close(fd[1]);

        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    RETURN(0);
    return 0;
}
