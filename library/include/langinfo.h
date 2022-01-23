/*
 * $Id: langinfo.h,v 1.0 2021-01-15 22:32:14 apalmate Exp $
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

#ifndef _LANGINFO_H
#define _LANGINFO_H

#include <features.h>

#include <nl_types.h>

/* Extract the category and item index from a constructed `nl_item' value.  */
#define _NL_ITEM_CATEGORY(item)		((int) (item) >> 16)
#define _NL_ITEM_INDEX(item)		((int) (item) & 0xffff)

#define	CODESET		1000	/* codeset name */
#define	D_T_FMT		1001	/* string for formatting date and time */
#define	D_FMT		1002	/* date format string */
#define	T_FMT		1003	/* time format string */
#define	T_FMT_AMPM	1004	/* a.m. or p.m. time formatting string */

#ifndef LIBRARIES_LOCALE_H
#define	AM_STR		41	/* Ante Meridian affix */
#define	PM_STR		42	/* Post Meridian affix */

/* week day names */
#define	DAY_1		1
#define	DAY_2		2
#define	DAY_3		3
#define	DAY_4		4
#define	DAY_5		5
#define	DAY_6		6
#define	DAY_7		7

/* abbreviated week day names */
#define	ABDAY_1		8
#define	ABDAY_2		9
#define	ABDAY_3		10
#define	ABDAY_4		11
#define	ABDAY_5		12
#define	ABDAY_6		13
#define	ABDAY_7		14

/* month names */
#define	MON_1		15
#define	MON_2		16
#define	MON_3		17
#define	MON_4		18
#define	MON_5		19
#define	MON_6		20
#define	MON_7		21
#define	MON_8		22
#define	MON_9		23
#define	MON_10		24
#define	MON_11		25
#define	MON_12		26

/* abbreviated month names */
#define	ABMON_1		27
#define	ABMON_2		28
#define	ABMON_3		29
#define	ABMON_4		30
#define	ABMON_5		31
#define	ABMON_6		32
#define	ABMON_7		33
#define	ABMON_8		34
#define	ABMON_9		35
#define	ABMON_10	36
#define	ABMON_11	37
#define	ABMON_12	38

#define	YESSTR		39	/* affirmative response for yes/no queries */
#define	NOSTR		40	/* negative response for yes/no queries */
#endif

#define	ERA		1045	/* era description segments */
#define	ERA_D_FMT	1046	/* era date format string */
#define	ERA_D_T_FMT	1047	/* era date and time format string */
#define	ERA_T_FMT	1048	/* era time format string */
#define	ALT_DIGITS	1049	/* alternative symbols for digits */

#define	RADIXCHAR	1050	/* radix char */
#define	THOUSEP		1051	/* separator for thousands */

#define	YESEXPR		1052	/* affirmative response expression */
#define	NOEXPR		1053	/* negative response expression */

#define	CRNCYSTR	1056	/* currency symbol */

#define	D_MD_ORDER	1057	/* month/day order (local extension) */

#define _NL_CTYPE_TRANSLIT_TAB_SIZE 1058
#define _NL_CTYPE_TRANSLIT_FROM_IDX 1059
#define _NL_CTYPE_TRANSLIT_FROM_TBL 1060
#define _NL_CTYPE_TRANSLIT_TO_IDX 1061
#define _NL_CTYPE_TRANSLIT_TO_TBL 1062
#define _NL_CTYPE_TRANSLIT_DEFAULT_MISSING_LEN 1063
#define _NL_CTYPE_TRANSLIT_DEFAULT_MISSING 1064
#define _NL_CTYPE_TRANSLIT_IGNORE_LEN 1065
#define _NL_CTYPE_TRANSLIT_IGNORE 1066
#define _NL_CTYPE_EXTRA_MAP_1 1070
#define _NL_CTYPE_EXTRA_MAP_2 1071
#define _NL_CTYPE_EXTRA_MAP_3 1072
#define _NL_CTYPE_EXTRA_MAP_4 1073
#define _NL_CTYPE_EXTRA_MAP_5 1074
#define _NL_CTYPE_EXTRA_MAP_6 1075
#define _NL_CTYPE_EXTRA_MAP_7 1076
#define _NL_CTYPE_EXTRA_MAP_8 1077
#define _NL_CTYPE_EXTRA_MAP_9 1078
#define _NL_CTYPE_EXTRA_MAP_10 1079
#define _NL_CTYPE_EXTRA_MAP_11 1080
#define _NL_CTYPE_EXTRA_MAP_12 1081
#define _NL_CTYPE_EXTRA_MAP_13 1082
#define _NL_CTYPE_EXTRA_MAP_14 1083

__BEGIN_DECLS
extern char *nl_langinfo(nl_item item);
__END_DECLS

#endif /* _LANGINFO_H */
