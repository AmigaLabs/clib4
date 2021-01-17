/*
 * $Id: langinfo_langinfo.c,v 1.0 2021-01-15 22:47:14 apalmate Exp $
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
 * build are available from <http://sourceforge.net/projects/clib2>.
 *
 *****************************************************************************
 */

/****************************************************************************/

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _TIMEZONE_HEADERS_H
#include "timezone_headers.h"
#endif /* _TIMEZONE_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

#include <locale.h>
#include <langinfo.h>

char *
nl_langinfo(nl_item item)
{
    ENTER();
    DECLARE_LOCALEBASE();

    const char *ret;
    char *s, *cs;
    static char *csym = NULL;
    static char *cset = NULL;

    switch (item)
    {
        case CODESET:
            ret = "";
            if ((s = setlocale(LC_CTYPE, NULL)) != NULL)
            {
                if ((cs = strchr(s, '.')) != NULL)
                {
                    ret = cs + 1;
                    if (strncmp(ret, "ISO_", 4) == 0)
                    {
                        int slen = strlen(ret);

                        if ((cset = realloc(cset, slen)) != NULL)
                        {
                            strcpy(cset, "ISO");
                            strcat(cset, ret + 4);
                            ret = cset;
                        }
                        else
                            ret = "";
                    }
                    else if (strcmp(ret, "EUC") == 0)
                    {
                        if (strncmp(s, "ja_JP", 5) == 0)
                            ret = "eucJP";
                        else if (strncmp(s, "ko_KR", 5) == 0)
                            ret = "eucKR";
                        else if (strncmp(s, "zh_CN", 5) == 0)
                            ret = "eucCN";
                    }
                    else if (strcmp(ret, "ASCII") == 0)
                        ret = "US-ASCII";
                }
                else if (strcmp(s, "C") == 0 ||
                        strcmp(s, "POSIX") == 0 || strstr(s, "ASCII") != NULL)
                    ret = "US-ASCII";
            }
            break;
        case D_T_FMT:
            ret = (char *) __default_locale->loc_DateTimeFormat;
            break;
        case D_FMT:
            ret = (char *) __default_locale->loc_DateFormat;
            break;
        case T_FMT:
            ret = (char *) __default_locale->loc_TimeFormat;
            break;            
        case AM_STR:
            ret = (char *) "AM"; // hardcoded
            break;
        case PM_STR:
            ret = (char *) "PM"; // hardcoded;
            break;
        case RADIXCHAR:
		    ret = (char*) __default_locale->loc_DecimalPoint;
		    break;
	    case THOUSEP:
		    ret = (char*) __default_locale->loc_GroupSeparator;
		    break;            
        default:
            ret = "";
    }
    
    RETURN(ret);
    return (char *) ret;
}
