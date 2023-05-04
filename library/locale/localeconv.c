/*
 * $Id: locale_localeconv.c,v 1.7 2006-09-25 14:51:15 clib2devs Exp $
*/

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

struct lconv *
localeconv(void)
{
	static struct lconv loc;

	struct Locale *numeric_locale;
	struct Locale *monetary_locale;
	struct lconv *result;

	ENTER();

	__locale_lock();

	numeric_locale = __CLIB2->__locale_table[LC_NUMERIC];
	monetary_locale = __CLIB2->__locale_table[LC_MONETARY];

	/* This makes up the current locale settings from the various
	   components in use. */
	loc.decimal_point = ((numeric_locale != NULL) ? ((char *)numeric_locale->loc_DecimalPoint) : (char *)".");
	loc.thousands_sep = ((numeric_locale != NULL) ? ((char *)numeric_locale->loc_GroupSeparator) : (char *)"");
	loc.grouping = ((numeric_locale != NULL) ? ((char *)numeric_locale->loc_Grouping) : (char *)"");
	loc.int_curr_symbol = ((monetary_locale != NULL) ? ((char *)monetary_locale->loc_MonIntCS) : (char *)"");
	loc.currency_symbol = ((monetary_locale != NULL) ? ((char *)monetary_locale->loc_MonCS) : (char *)"");
	loc.mon_decimal_point = ((monetary_locale != NULL) ? ((char *)monetary_locale->loc_MonDecimalPoint) : (char *)"");
	loc.mon_thousands_sep = ((monetary_locale != NULL) ? ((char *)monetary_locale->loc_MonGroupSeparator) : (char *)"");
	loc.mon_grouping = ((monetary_locale != NULL) ? ((char *)monetary_locale->loc_MonGrouping) : (char *)"");
	loc.positive_sign = ((monetary_locale != NULL) ? ((char *)monetary_locale->loc_MonPositiveSign) : (char *)"");
	loc.negative_sign = ((monetary_locale != NULL) ? ((char *)monetary_locale->loc_MonNegativeSign) : (char *)"");

	loc.int_frac_digits = (monetary_locale != NULL) ? monetary_locale->loc_MonIntFracDigits : CHAR_MAX;
	loc.frac_digits = (monetary_locale != NULL) ? monetary_locale->loc_MonFracDigits : CHAR_MAX;
	loc.p_cs_precedes = (monetary_locale != NULL) ? monetary_locale->loc_MonPositiveCSPos : CHAR_MAX;
	loc.p_sep_by_space = (monetary_locale != NULL) ? monetary_locale->loc_MonPositiveSpaceSep : CHAR_MAX;
	loc.n_cs_precedes = (monetary_locale != NULL) ? monetary_locale->loc_MonNegativeCSPos : CHAR_MAX;
	loc.n_sep_by_space = (monetary_locale != NULL) ? monetary_locale->loc_MonNegativeSpaceSep : CHAR_MAX;
	loc.p_sign_posn = (monetary_locale != NULL) ? monetary_locale->loc_MonPositiveSignPos : CHAR_MAX;
	loc.n_sign_posn = (monetary_locale != NULL) ? monetary_locale->loc_MonNegativeSignPos : CHAR_MAX;

	result = &loc;

	__locale_unlock();

	RETURN(result);
	return (result);
}
