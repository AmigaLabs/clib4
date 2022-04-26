/*
 * $Id: wctype.h,v 1.9 2022-04-20 14:41:28 clib2devs Exp $
*/

#ifndef _WCYTPE_H
#define _WCYTPE_H

#ifndef _WCHAR_H
#include <wchar.h>
#endif /* _WCHAR_H */

#include <features.h>

__BEGIN_DECLS

typedef long wctype_t; /* ZZZ */
typedef long wctrans_t; /* ZZZ */

extern int iswalnum(wint_t c);
extern int iswalpha(wint_t c);
extern int iswcntrl(wint_t c);
extern int iswdigit(wint_t c);
extern int iswxdigit(wint_t c);
extern int iswgraph(wint_t c);
extern int iswpunct(wint_t c);
extern int iswprint(wint_t c);
extern int iswlower(wint_t c);
extern int iswupper(wint_t c);
extern int iswspace(wint_t c);
extern int iswblank(wint_t c);
extern wint_t towlower(wint_t c);
extern wint_t towupper(wint_t c);
extern wctype_t wctype(const char *property);
extern int iswctype(wint_t c, wctype_t desc);
extern wctrans_t wctrans(const char *property);
extern wint_t towctrans(wint_t c, wctrans_t desc);

__END_DECLS

#endif /* _WCYTPE_H */
