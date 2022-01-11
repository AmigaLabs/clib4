/*
 * $Id: stdio_tempnam.c,v 1.0 2021-02-22 17:46:25 apalmate Exp $
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

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/* Try to open the file specified, if it can't be opened then try
   another one.  Return nonzero if successful, otherwise zero.  */

static int
worker(char *result, const char *part1, const char *part2, char *part3, int *part4)
{
    /*  Generate the filename and make sure that there isn't one called it already.  */

    while (1)
    {
        if (__check_abort_enabled)
            __check_abort();

        int t;

        if (!__global_clib2->__unix_path_semantics)
        {
            if (!strcmp(part1, P_tmpdir))
            {
                sprintf(result, "T:%s%s.%x", part2, part3, *part4);
            }
            else
            {
                size_t len = strlen(part1);

                if (len == 0 || part1[len - 1] == ':' || part1[len - 1] == '/')
                {
                    sprintf(result, "%s%s%s.%x", part1, part2, part3, *part4);
                }
                else
                {
                    sprintf(result, "%s/%s%s.%x", part1, part2, part3, *part4);
                }
            }
        }
        else
            sprintf(result, "%s/%s%s.%x", part1, part2, part3, *part4);
        (*part4)++;
        t = open(result, O_RDONLY, 0);
        if (t == -1)
        {
            if (errno == ENOSYS)
            {
                result[0] = '\0';
                return 0;
            }
            break;
        }
        close(t);
    }
    return 1;
}

char *
tempnam(const char *dir, const char *pfx)
{
    ENTER();
    
    if (__check_abort_enabled)
        __check_abort();

    char *filename;
    int length;
    const char *prefix = (pfx) ? pfx : "";

    if (dir == NULL && (dir = getenv("TMPDIR")) == NULL)
        dir = P_tmpdir;

    /* two 8 digit numbers + . / */
    length = strlen(dir) + strlen(prefix) + (4 * sizeof(int)) + 2 + 1;

    filename = malloc(length);
    if (filename)
    {
        int r = rand();
        char string[7] = {0};

        snprintf (string, 7, "%lX", r);        
        if (!worker(filename, dir, prefix, string, &__global_clib2->inc))
            return NULL;
    }

    RETURN(filename);
    return filename;
}
