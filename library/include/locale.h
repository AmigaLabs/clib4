/*
 * $Id: locale.h,v 1.5 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _LOCALE_H
#define _LOCALE_H

#include <features.h>

__BEGIN_DECLS

#define LC_ALL		0	/* all behaviour */
#define LC_COLLATE	1	/* behaviour of strcoll() and strxfrm() */
#define LC_CTYPE	2	/* character handling */
#define LC_MONETARY	3	/* monetary information returned by localeconv() */
#define LC_NUMERIC	4	/* decimal point and non-monetary information returned by localeconv() */
#define LC_TIME		5	/* behaviour of strftime() */
#define LC_MESSAGES 6

#define LC_MIN          LC_ALL
#define LC_MAX          LC_MESSAGES

typedef void *locale_t;

struct lconv
{
	char *	decimal_point;		/* Decimal point character (non-monetary). */
	char *	thousands_sep;		/* Non-monetary digit group separator character(s). */
	char *	grouping;			/* Non-monetary digit groupings. */
	char *	int_curr_symbol;	/* International currency symbol, followed by the character that separates it from the monetary quantity. */
	char *	currency_symbol;	/* The local currency symbol for the current locale. */
	char *	mon_decimal_point;	/* Decimal point character for monetary quantities. */
	char *	mon_thousands_sep;	/* Monetary digit group separator character(s). */
	char *	mon_grouping;		/* Monetary digit groupings. */
	char *	positive_sign;		/* Sign character(s) for non-negative monetary quantities. */
	char *	negative_sign;		/* Sign character(s) for negative monetary quantities. */
	char	int_frac_digits;	/* Digits shown right of the decimal point in international monetary format. */
	char	frac_digits;		/* Digits shown to the right of the decimal point in other monetary formats. */
	char	p_cs_precedes;		/* If currency symbol precedes non-negative monetary values this will be 1, otherwise it will be 0. */
	char	p_sep_by_space;		/* If currency symbol is separated from non-negative monetary values by a blank space this will be 1, otherwise it will be 0. */
	char	n_cs_precedes;		/* If currency symbol precedes negative monetary values this will be 1, otherwise it will be 0. */
	char	n_sep_by_space;		/* If currency symbol is separated from negative monetary values by a blank space this will be 1, otherwise it will be 0. */
	char	p_sign_posn;		/* Where to place the positive sign and the currency symbol for a non-negative monetary quantity. */
	char	n_sign_posn;		/* Where to place the negative sign and the currency symbol for a negative monetary quantity. */
};

extern char *setlocale(int category, const char *locale);
extern struct lconv *localeconv(void);

__END_DECLS

#endif /* _LOCALE_H */
