/*
 * $Id: unistd_spawn.c,v 1.0 2021-01-21 18:26:47 apalmate Exp $
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

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

/* The following is not part of the ISO 'C' (1994) standard. */

/****************************************************************************/

STATIC BOOL
string_needs_quoting(const char *string, size_t len)
{
    BOOL result = FALSE;
    size_t i;
    char c;

    for (i = 0; i < len; i++)
    {
        c = (*string++);
        if (c == ' ' || ((unsigned char)c) == 0xA0 || c == '\t' || c == '\n' || c == '\"')
        {
            result = TRUE;
            break;
        }
    }

    return (result);
}

STATIC void
build_arg_string(char *const argv[], char *arg_string)
{
    BOOL first_char = TRUE;
    size_t i, j, len;
    char *s;

    /* The first argv[] element is skipped; it does not contain part of
	   the command line but holds the name of the program to be run. */
    for (i = 1; argv[i] != NULL; i++)
    {
        s = (char *)argv[i];

        len = strlen(s);
        if (len > 0)
        {
            if (first_char)
                first_char = FALSE;
            else
                (*arg_string++) = ' ';

            if ((*s) != '\"' && string_needs_quoting(s, len))
            {
                (*arg_string++) = '\"';

                for (j = 0; j < len; j++)
                {
                    if (s[j] == '\"' || s[j] == '*')
                    {
                        (*arg_string++) = '*';
                        (*arg_string++) = s[j];
                    }
                    else if (s[j] == '\n')
                    {
                        (*arg_string++) = '*';
                        (*arg_string++) = 'N';
                    }
                    else
                    {
                        (*arg_string++) = s[j];
                    }
                }

                (*arg_string++) = '\"';
            }
            else
            {
                memcpy(arg_string, s, len);
                arg_string += len;
            }
        }
    }
}

STATIC size_t
count_extra_escape_chars(const char *string, size_t len)
{
    size_t count = 0;
    size_t i;
    char c;

    for (i = 0; i < len; i++)
    {
        c = (*string++);
        if (c == '\"' || c == '*' || c == '\n')
            count++;
    }

    return (count);
}

STATIC size_t
get_arg_string_length(char *const argv[])
{
    size_t result = 0;
    size_t i, len;
    char *s;

    /* The first argv[] element is skipped; it does not contain part of
	   the command line but holds the name of the program to be run. */
    for (i = 1; argv[i] != NULL; i++)
    {
        s = (char *)argv[i];

        len = strlen(s);
        if (len > 0)
        {
            if ((*s) != '\"')
            {
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

int spawnv(int mode, const char *file, const char *const *argv)
{
    int ret = -1;
    char *arg_string = NULL;
    size_t arg_string_len = 0;
    size_t parameter_string_len = 0;

#if defined(UNIX_PATH_SEMANTICS)
    struct name_translation_info path_nti;
#endif /* UNIX_PATH_SEMANTICS */

    if (mode != P_WAIT)
    {
        __set_errno(ENOSYS);
        return ret;
    }

    __set_errno(0);

#if defined(UNIX_PATH_SEMANTICS)
    if (__translate_unix_to_amiga_path_name(file, &path_nti) != 0)
    {
        __set_errno(EINVAL);
        return ret;
    }
#endif

    parameter_string_len = get_arg_string_length((char *const *)argv);
    if (parameter_string_len > _POSIX_ARG_MAX)
    {
        __set_errno(E2BIG);
        return ret;
    }

    arg_string = malloc(parameter_string_len + 1 + 1);
    if (arg_string == NULL)
    {
        __set_errno(ENOMEM);
        return ret;
    }

    if (parameter_string_len > 0)
    {
        build_arg_string((char *const *)argv, &arg_string[arg_string_len]);
        arg_string_len += parameter_string_len;
    }

    /* Add the terminating new line character and a NUL, to be nice... */
    arg_string[arg_string_len++] = '\n';
    arg_string[arg_string_len] = '\0';

    int pathlen = strlen(file) + strlen(arg_string) + 1;
    char *finalpath = calloc(1, pathlen);
    strncpy(finalpath, file, strlen(file));
    strncat(finalpath, arg_string, strlen(arg_string));
    finalpath[pathlen] = '\0';

    ret = SystemTags(finalpath,
                     SYS_Input, 0,
                     SYS_Output, 0,
                     SYS_UserShell, TRUE,
                     SYS_Asynch, TRUE,
                     SYS_UserShell, TRUE,
                     TAG_DONE);

    if (ret != 0)
    {
        errno = __translate_io_error_to_errno(IoErr());
    }
    else
    {
        /* From DOS 51.77 if an ASYNC command was started ok (returncode 0) the PID is in IoErr() */
        ret = IoErr();
    }

    return ret;
}