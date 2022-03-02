/*
 * $Id: time_utimes.c,v 1.0 2022-02-24 18:08:57 apalmate Exp $
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

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <sys/time.h>

int utimes(const char *name, const struct timeval *tvp) {
#if defined(UNIX_PATH_SEMANTICS)
    struct name_translation_info path_name_nti;
#endif /* UNIX_PATH_SEMANTICS */
    struct DateStamp ds;

    ENTER();

    if (__check_abort_enabled)
        __check_abort();

    DECLARE_TIMEZONEBASE();

    if (!tvp) {
        DateStamp(&ds);
    } else {
        time_t mtime = tvp[1].tv_sec;
        int32 __gmtoffset = 0;

        if (UNIX_TIME_OFFSET > mtime) {
            __set_errno(EINVAL);
            return -1;
        }

        GetTimezoneAttrs(NULL, TZA_UTCOffset, &__gmtoffset, TAG_DONE);

        mtime -= UNIX_TIME_OFFSET;
        mtime -= 60 * __gmtoffset;

        ds.ds_Days = mtime / (24 * 60 * 60);
        ds.ds_Minute = (mtime % (24 * 60 * 60)) / 60;
        ds.ds_Tick = (mtime % 60) * TICKS_PER_SECOND
                     + tvp[1].tv_usec / (1000000 / TICKS_PER_SECOND);

        while (ds.ds_Tick >= 60 * TICKS_PER_SECOND) {
            ds.ds_Tick -= 60 * TICKS_PER_SECOND;
            ds.ds_Minute++;
        }
        if (ds.ds_Minute >= 24 * 60) {
            ds.ds_Minute -= 24 * 60;
            ds.ds_Days++;
        }
    }

#if defined(UNIX_PATH_SEMANTICS)
    if (__global_clib2->__unix_path_semantics) {
        if (__translate_unix_to_amiga_path_name(&name, &path_name_nti) != 0) {
            return -1;
        }

        if (path_name_nti.is_root) {
            __set_errno(EPERM);
            return -1;
        }
    }
#endif

    if (!SetDate((STRPTR) name, &ds)) {
        __set_errno(__translate_io_error_to_errno(IoErr()));
        return -1;
    }

    return 0;
}