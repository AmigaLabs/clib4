/*
 * $Id: ctype.h,v 1.10 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _CTYPE_H
#define _CTYPE_H

#include <features.h>
#include <dos.h>

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
/****************************************************************************/

/*
 * If requested, reimplement the character classification functions as macros;
 * note that the macro variants ignore the current locale and default to the
 * 'C' locale rules.
 */

#ifndef __cplusplus

#define __CTYPE_CONTROL		0x01	/* This is a control character */
#define __CTYPE_DIGIT		0x02	/* This is a 'decimal' digit */
#define __CTYPE_HEX_DIGIT	0x04	/* This is a hexadecimal digit */
#define __CTYPE_PUNCTUATION	0x08	/* This is a punctuation character */
#define __CTYPE_PRINTABLE	0x10	/* This is a printable character */
#define __CTYPE_WHITE_SPACE	0x20	/* This is a blank space character */
#define __CTYPE_LOWER_CASE	0x40	/* This is a lower case letter */
#define __CTYPE_UPPER_CASE	0x80	/* This is an upper case letter */

/****************************************************************************/

#define isalnum(c)	((__CLIB2->__ctype_table[(c) & 255] & (__CTYPE_DIGIT|__CTYPE_LOWER_CASE|__CTYPE_UPPER_CASE)) != 0)
#define isalpha(c)	((__CLIB2->__ctype_table[(c) & 255] & (__CTYPE_LOWER_CASE|__CTYPE_UPPER_CASE)) != 0)
#define iscntrl(c)	((__CLIB2->__ctype_table[(c) & 255] & __CTYPE_CONTROL) != 0)
#define isdigit(c)	((__CLIB2->__ctype_table[(c) & 255] & __CTYPE_DIGIT) != 0)
#define isxdigit(c)	((__CLIB2->__ctype_table[(c) & 255] & __CTYPE_HEX_DIGIT) != 0)
#define isgraph(c)	((__CLIB2->__ctype_table[(c) & 255] & (__CTYPE_DIGIT|__CTYPE_PUNCTUATION|__CTYPE_LOWER_CASE|__CTYPE_UPPER_CASE)) != 0)
#define ispunct(c)	((__CLIB2->__ctype_table[(c) & 255] & __CTYPE_PUNCTUATION) != 0)
#define isprint(c)	((__CLIB2->__ctype_table[(c) & 255] & __CTYPE_PRINTABLE) != 0)
#define islower(c)	((__CLIB2->__ctype_table[(c) & 255] & __CTYPE_LOWER_CASE) != 0)
#define isupper(c)	((__CLIB2->__ctype_table[(c) & 255] & __CTYPE_UPPER_CASE) != 0)
#define isspace(c)	((__CLIB2->__ctype_table[(c) & 255] & __CTYPE_WHITE_SPACE) != 0)
#define isblank(c)	((c) == ' ' || (c) == '\t')

#endif /* __cplusplus */

__END_DECLS

#endif /* _CTYPE_H */
