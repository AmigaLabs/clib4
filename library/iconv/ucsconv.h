/*
 * Copyright (c) 2003-2004, Artem B. Bityuckiy
 * Copyright (c) 1999,2000, Konstantin Chuguev. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef __ICONV_UCS_CONVERSION_H__
#define __ICONV_UCS_CONVERSION_H__

#include <sys/types.h>
#include <wchar.h>
#include "local.h"

/* No enough space in output buffer */
#define ICONV_CES_NOSPACE 0
/* Invalid input character */
#define ICONV_CES_INVALID_CHARACTER -1
/* No corespondent character in destination encoding */
#define ICONV_CES_BAD_SEQUENCE -2
/* All unknown characters are marked by this code */
#define DEFAULT_CHARACTER 0x3f /* ASCII '?' */

/*
 * iconv_to_ucs_ces_handlers_t - "to UCS" CES converter handlers.
 *
 * Structure contains function pointers which should be provided by
 * "to_ucs" CES converter.
 *
 * ============================================================================
 */
typedef struct
{
    void *(*init)(const char *encoding);
    size_t (*close)(void *data);
    int (*get_mb_cur_max)(void *data);
    void (*get_state)(void *data, mbstate_t *state);
    int (*set_state)(void *data, mbstate_t *state);
    int (*is_stateful)(void *data);
    ucs4_t (*convert_to_ucs)(void *data, const unsigned char **inbuf, size_t *inbytesleft);
} iconv_to_ucs_ces_handlers_t;

typedef struct
{
    /* Same as in iconv_to_ucs_ces_handlers_t */
    void *(*init)(const char *encoding);

    /* Same as in iconv_to_ucs_ces_handlers_t */
    size_t (*close)(void *data);

    /* Same as in iconv_to_ucs_ces_handlers_t */
    int (*get_mb_cur_max)(void *data);

    /* Same as in iconv_to_ucs_ces_handlers_t */
    void (*get_state)(void *data, mbstate_t *state);

    /* Same as in iconv_to_ucs_ces_handlers_t */
    int (*set_state)(void *data, mbstate_t *state);

    /* Same as in iconv_to_ucs_ces_handlers_t */
    int (*is_stateful)(void *data);

    size_t (*convert_from_ucs)(void *data, ucs4_t in, unsigned char **outbuf, size_t *outbytesleft);
} iconv_from_ucs_ces_handlers_t;

/*
 * iconv_to_ucs_ces_desc_t - "to UCS" CES converter definition structure for
 * usage in iconv_ucs_conversion_t conversion description structure.
 *
 * ============================================================================
 */
typedef struct
{
    /* CES converter handlers */
    const iconv_to_ucs_ces_handlers_t *handlers;

    /* "to_ucs" CES converter-specific data. */
    void *data;
} iconv_to_ucs_ces_desc_t;

/*
 * iconv_from_ucs_ces_desc_t - "from UCS" CES converter definition structure for
 * usage in iconv_ucs_conversion_t conversion description structure.
 *
 * ============================================================================
 */
typedef struct
{
    /* CES converter handlers */
    const iconv_from_ucs_ces_handlers_t *handlers;

    /* "from_ucs" CES converter-specific data. */
    void *data;
} iconv_from_ucs_ces_desc_t;

/*
 * iconv_ucs_conversion_t - UCS-based conversion definition structure.
 *
 * Defines special type of conversion where every character is first
 * converted into UCS-4 (UCS-2 for table-driven), and after this the
 * resulting UCS character is converted to destination encoding. 
 * UCS-based conversion is composed of two *converters*, defined by 
 * iconv_ces_t structure. The iconv_ucs_conversion_t object is referred
 * from iconv_conversion_t object using 'data' field.
 *
 * Structure contains two objects - 'to_ucs' and 'from_ucs' which define
 * "source encoding to UCS" and "UCS to destination encoding" converters.
 *
 * ============================================================================
 */
typedef struct
{
    /* Source encoding -> CES converter. */
    iconv_to_ucs_ces_desc_t to_ucs;

    /* UCS -> destination encoding CES converter. */
    iconv_from_ucs_ces_desc_t from_ucs;
} iconv_ucs_conversion_t;

/*
 * iconv_to_ucs_ces_t - defines "to UCS" CES converter.
 *
 * ============================================================================
 */
typedef struct
{
    /* 
   * An array of encodings names, supported by CES converter.
   * The end of array should be marked by NULL pointer.
   */
    const char **names;

    /* CES converter description structure */
    const iconv_to_ucs_ces_handlers_t *handlers;
} iconv_to_ucs_ces_t;

/*
 * iconv_from_ucs_ces_t - defines "from UCS" CES converter.
 *
 * ============================================================================
 */
typedef struct
{
    /* 
   * An array of encodings names, supported by CES converter.
   * The end of array should be marked by NULL pointer.
   */
    const char **names;

    /* CES converter description structure */
    const iconv_from_ucs_ces_handlers_t *handlers;
} iconv_from_ucs_ces_t;

/* List of "to UCS" linked-in CES converters. */
extern const iconv_to_ucs_ces_t
    _iconv_to_ucs_ces[];

/* List of "from UCS" linked-in CES converters. */
extern const iconv_from_ucs_ces_t
    _iconv_from_ucs_ces[];

#endif /* !__ICONV_UCS_CONVERSION_H__ */
