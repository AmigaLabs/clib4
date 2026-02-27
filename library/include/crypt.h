/*
 * UFC-crypt: ultra fast crypt(3) implementation
 *
 * Copyright (C) 1991-2022 Free Software Foundation, Inc.
 *
 * The GNU C Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, see
 * <https://www.gnu.org/licenses/>.
 *
 * @(#)crypt.h	1.5 12/20/96
 *
 */

#ifndef _CRYPT_H
#define _CRYPT_H    1

#include <features.h>

__BEGIN_DECLS

/* This structure provides scratch and output buffers for 'crypt_r'.
   Its contents should not be accessed directly.  */
struct crypt_data {
    int initialized;
    char __buf[256];
};

/* Thread-safe version of 'crypt'.
   DATA must point to a 'struct crypt_data' allocated by the caller.
   Before the first call to 'crypt_r' with a new 'struct crypt_data',
   that object must be initialized to all zeroes.  The pointer
   returned, if not NULL, will point within DATA.  (It will still be
   overwritten by the next call to 'crypt_r' with the same DATA.)  */
extern char *crypt_r (const char *__phrase, const char *__salt, struct crypt_data * __restrict __data);

__END_DECLS

#endif    /* crypt.h */
