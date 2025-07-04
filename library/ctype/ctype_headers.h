/*
 * $Id: ctype_headers.h,v 1.6 2006-01-08 12:04:22 clib4devs Exp $
*/
#ifndef _CTYPE_HEADERS_H
#define _CTYPE_HEADERS_H
#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */
#ifndef _STDLIB_LOCALEBASE_H
#include "stdlib_localebase.h"
#endif /* _STDLIB_LOCALEBASE_H */
#ifndef _STDLIB_UTILITYBASE_H
#include "stdlib_utilitybase.h"
#endif /* _STDLIB_UTILITYBASE_H */
#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */
/****************************************************************************/
#include <locale.h>
#include <assert.h>
#include <limits.h>
#include <ctype.h>

struct __locale_t {
	char categories[__LC_LAST][ENCODING_LEN + 1];
	char locale_string[__LC_LAST * (ENCODING_LEN + 1/*"/"*/ + 1)];

	int (* wctomb)(struct _clib4 *, char *, wchar_t, mbstate_t *);

	int (* mbtowc)(struct _clib4 *, wchar_t *, const char *, size_t, mbstate_t *);

	int cjk_lang;
	char *ctype_ptr;
	struct lconv lconv;
	char mb_cur_max[2];
	char ctype_codeset[ENCODING_LEN + 1];
	char message_codeset[ENCODING_LEN + 1];
};

int __iso_8859_val_index(int val);

int __iso_8859_index(const char *charset_ext);

int __cp_val_index(int val);

int __cp_index(const char *charset_ext);
#endif /* _CTYPE_HEADERS_H */
