/*
 * $Id: argz.h,v 1.0 2021-01-21 10:03:49 apalmate Exp $
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
 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 */
#ifndef _ARGZ_H
#define _ARGZ_H

#include <sys/types.h>

/****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/****************************************************************************/
#ifndef __error_t_defined
#define __error_t_defined 1
    typedef int error_t;
#endif

/****************************************************************************/

extern error_t argz_create(char *const argv[], char **argz, size_t *argz_len);
extern error_t argz_create_sep(const char *string, int sep, char **argz, size_t *argz_len);
extern size_t argz_count(const char *argz, size_t argz_len);
extern void argz_extract(char *argz, size_t argz_len, char **argv);
extern void argz_stringify(char *argz, size_t argz_len, int sep);
extern error_t argz_add(char **argz, size_t *argz_len, const char *str);
extern error_t argz_add_sep(char **argz, size_t *argz_len, const char *str, int sep);
extern error_t argz_append(char **argz, size_t *argz_len, const char *buf, size_t buf_len);
extern error_t argz_delete(char **argz, size_t *argz_len, char *entry);
extern error_t argz_insert(char **argz, size_t *argz_len, char *before, const char *entry);
extern char *argz_next(char *argz, size_t argz_len, const char *entry);
extern error_t argz_replace(char **argz, size_t *argz_len, const char *str, const char *with, unsigned *replace_count);

/****************************************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

/****************************************************************************/

#endif // _ARGZ_H