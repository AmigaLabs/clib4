/*
 * $Id: ctype.h,v 1.10 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef _CTYPE_H
#define _CTYPE_H

#include <features.h>
#include <locale.h>

__BEGIN_DECLS

extern int isalnum(int c);
extern int isalpha(int c);
extern int iscntrl(int c);
extern int isdigit(int c);
extern int isgraph(int c);
extern int islower(int c);
extern int isprint(int c);
extern int ispunct(int c);
extern int isspace(int c);
extern int isupper(int c);
extern int isxdigit(int c);
extern int tolower(int c);
extern int toupper(int c);

extern int isblank(int c);
extern int isascii(int c);
extern int toascii(int c);

#undef _P

#define	_U	01
#define	_L	02
#define	_N	04
#define	_S	010
#define _P	020
#define _C	040
#define _X	0100
#define	_B	0200

extern const char *__ctype_ptr;
extern const char _ctype_[];  /* For backward compatibility.  */

#ifndef __cplusplus

#define	isalpha(c)	((__ctype_ptr)[(unsigned)(c)]&(_U|_L))
#define	isupper(c)	((__ctype_ptr)[(unsigned)(c)]&_U)
#define	islower(c)	((__ctype_ptr)[(unsigned)(c)]&_L)
#define	isdigit(c)	((__ctype_ptr)[(unsigned)(c)]&_N)
#define	isxdigit(c)	((__ctype_ptr)[(unsigned)(c)]&(_X|_N))
#define	isspace(c)	((__ctype_ptr)[(unsigned)(c)]&_S)
#define ispunct(c)	((__ctype_ptr)[(unsigned)(c)]&_P)
#define isalnum(c)	((__ctype_ptr)[(unsigned)(c)]&(_U|_L|_N))
#define isprint(c)	((__ctype_ptr)[(unsigned)(c)]&(_P|_U|_L|_N|_B))
#define	isgraph(c)	((__ctype_ptr)[(unsigned)(c)]&(_P|_U|_L|_N))
#define iscntrl(c)	((__ctype_ptr)[(unsigned)(c)]&_C)

#ifdef __GNUC__
# define toupper(c) \
	__extension__ ({ int __x = (c); islower(__x) ? (__x - 'a' + 'A') : __x;})
# define tolower(c) \
	__extension__ ({ int __x = (c); isupper(__x) ? (__x - 'A' + 'a') : __x;})
#endif /* !__GNUC__ */

#endif /* !__cplusplus */

__END_DECLS

#endif /* _CTYPE_H */
