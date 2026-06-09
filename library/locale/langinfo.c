/*
 * $Id: langinfo_langinfo.c,v 1.0 2021-01-15 22:47:14 clib4devs Exp $
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

/* nl_langinfo() — return locale-specific information string.
 *
 * Bridges AmigaOS locale.library data to POSIX nl_item queries.
 * Uses the system default locale (__default_locale) which is opened
 * during clib4 initialization. For day/month/AM-PM/yes-no strings,
 * calls GetLocaleStr() and falls back to C-locale English defaults
 * when the language driver returns empty (as the English driver does).
 */
char *
nl_langinfo(nl_item item) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;
    DECLARE_LOCALEBASE_R(__clib4);
    DECLARE_FONTBASE_R(__clib4);

    const char *ret = NULL;
    char *cs;
    static char *cset = NULL;
    /* Static buffers for synthesized YESEXPR/NOEXPR/CRNCYSTR strings */
    static char yesexpr_buf[8];
    static char noexpr_buf[8];
    static char crncystr_buf[16];

    switch (item) {
        /* CODESET: extract charset name from _current_locale string.
         * The locale name is in "C-<encoding>" format (e.g. "C-UTF-8",
         * "C-ISO-8859-1"). Parse out the encoding part after the '-'.
         * If no "C-" prefix, query diskfont.library for the MIME name
         * of the locale's loc_CodeSet number.
         */
        case CODESET:
            ret = "";
            const char *s = __clib4->_current_locale;
            if (strstr(s, "C-")) {
                if ((cs = strchr(s, '-')) != NULL) {
                    ret = cs + 1;
                    if (strncmp(ret, "ISO_", 4) == 0) {
                        int slen = strlen(ret);
                        char *tmp = realloc(cset, slen);

                        if (tmp != NULL) {
                            cset = tmp;
                            strcpy(cset, "ISO");
                            strcat(cset, ret + 4);
                            ret = cset;
                        } else {
                            free(cset);
                            ret = "";
                        }
                    } else if (strcmp(ret, "EUC") == 0) {
                        if (strncmp(s, "ja_JP", 5) == 0)
                            ret = "eucJP";
                        else if (strncmp(s, "ko_KR", 5) == 0)
                            ret = "eucKR";
                        else if (strncmp(s, "zh_CN", 5) == 0)
                            ret = "eucCN";
                    } else if (strcmp(ret, "UTF-8") == 0)
                        ret = "UTF-8";
                    else if (strcmp(ret, "ASCII") == 0)
                        ret = "US-ASCII";
                } else if (strcmp(s, "C") == 0 ||
                           strcmp(s, "POSIX") == 0 || strstr(s, "ASCII") != NULL)
                    ret = "US-ASCII";

                if (ret == NULL) {
                    uint32 default_charset = __clib4->__default_locale->loc_CodeSet;
                    ret = (char *) ObtainCharsetInfo(DFCS_NUMBER, default_charset, DFCS_MIMENAME);
                }
            } else {
                uint32 default_charset = __clib4->__default_locale->loc_CodeSet;
                ret = (char *) ObtainCharsetInfo(DFCS_NUMBER, default_charset, DFCS_MIMENAME);
            }
            break;
        case D_T_FMT:
            ret = (char *) __clib4->__default_locale->loc_DateTimeFormat;
            break;
        case D_FMT:
            ret = (char *) __clib4->__default_locale->loc_DateFormat;
            break;
        case T_FMT:
            ret = (char *) __clib4->__default_locale->loc_TimeFormat;
            break;
        /* T_FMT_AMPM: AmigaOS locale doesn't have a separate AM/PM time
         * format, so use the standard POSIX 12-hour format string.
         */
        case T_FMT_AMPM:
            ret = "%I:%M:%S %p";
            break;

        /* Day names, abbreviated day names, month names, abbreviated month
         * names, AM/PM strings, and yes/no strings: bridge directly to
         * locale.library's GetLocaleStr(). The POSIX nl_item constants
         * (DAY_1=1, ABDAY_1=8, MON_1=15, etc.) match the AmigaOS
         * GetLocaleStr() string codes by design.
         */
        case DAY_1: case DAY_2: case DAY_3: case DAY_4:
        case DAY_5: case DAY_6: case DAY_7:
        case ABDAY_1: case ABDAY_2: case ABDAY_3: case ABDAY_4:
        case ABDAY_5: case ABDAY_6: case ABDAY_7:
        case MON_1: case MON_2: case MON_3: case MON_4:
        case MON_5: case MON_6: case MON_7: case MON_8:
        case MON_9: case MON_10: case MON_11: case MON_12:
        case ABMON_1: case ABMON_2: case ABMON_3: case ABMON_4:
        case ABMON_5: case ABMON_6: case ABMON_7: case ABMON_8:
        case ABMON_9: case ABMON_10: case ABMON_11: case ABMON_12:
        case AM_STR: case PM_STR:
        case YESSTR: case NOSTR:
            if (__clib4->__default_locale != NULL && ILocale != NULL) {
                ret = (const char *) GetLocaleStr(
                    __clib4->__default_locale, (uint32) item);
            }
            /* Fall back to C-locale English defaults if GetLocaleStr
             * returned NULL or empty. The English language driver returns
             * empty strings because English is the "built-in" language
             * and callers are expected to provide their own defaults.
             */
            if (ret == NULL || ret[0] == '\0') {
                static const char * const c_day_names[] = {
                    "Sunday", "Monday", "Tuesday", "Wednesday",
                    "Thursday", "Friday", "Saturday"
                };
                static const char * const c_abday_names[] = {
                    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
                };
                static const char * const c_mon_names[] = {
                    "January", "February", "March", "April",
                    "May", "June", "July", "August",
                    "September", "October", "November", "December"
                };
                static const char * const c_abmon_names[] = {
                    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
                };

                if (item >= DAY_1 && item <= DAY_7)
                    ret = c_day_names[item - DAY_1];
                else if (item >= ABDAY_1 && item <= ABDAY_7)
                    ret = c_abday_names[item - ABDAY_1];
                else if (item >= MON_1 && item <= MON_12)
                    ret = c_mon_names[item - MON_1];
                else if (item >= ABMON_1 && item <= ABMON_12)
                    ret = c_abmon_names[item - ABMON_1];
                else switch (item) {
                    case AM_STR:  ret = "AM"; break;
                    case PM_STR:  ret = "PM"; break;
                    case YESSTR:  ret = "yes"; break;
                    case NOSTR:   ret = "no"; break;
                    default:      ret = ""; break;
                }
            }
            break;

        /* YESEXPR/NOEXPR: synthesize POSIX regex from the locale's
         * yes/no response strings.
         */
        case YESEXPR:
            if (__clib4->__default_locale != NULL && ILocale != NULL) {
                const char *yes = (const char *) GetLocaleStr(
                    __clib4->__default_locale, YESSTR);
                if (yes != NULL && yes[0] != '\0')
                    snprintf(yesexpr_buf, sizeof(yesexpr_buf),
                             "^[%c%c]", yes[0],
                             (yes[0] >= 'a' && yes[0] <= 'z') ?
                                 yes[0] - 32 : yes[0] + 32);
                else
                    strcpy(yesexpr_buf, "^[yY]");
            } else {
                strcpy(yesexpr_buf, "^[yY]");
            }
            ret = yesexpr_buf;
            break;
        case NOEXPR:
            if (__clib4->__default_locale != NULL && ILocale != NULL) {
                const char *no = (const char *) GetLocaleStr(
                    __clib4->__default_locale, NOSTR);
                if (no != NULL && no[0] != '\0')
                    snprintf(noexpr_buf, sizeof(noexpr_buf),
                             "^[%c%c]", no[0],
                             (no[0] >= 'a' && no[0] <= 'z') ?
                                 no[0] - 32 : no[0] + 32);
                else
                    strcpy(noexpr_buf, "^[nN]");
            } else {
                strcpy(noexpr_buf, "^[nN]");
            }
            ret = noexpr_buf;
            break;

        /* CRNCYSTR: currency symbol prefixed with '-' if it precedes
         * the value, '+' if it succeeds, or '.' if it replaces the
         * radix character.
         */
        case CRNCYSTR:
            if (__clib4->__default_locale != NULL) {
                const char *sym = (const char *)
                    __clib4->__default_locale->loc_MonCS;
                uint8 pos = __clib4->__default_locale->loc_MonPositiveCSPos;
                if (sym != NULL && sym[0] != '\0') {
                    snprintf(crncystr_buf, sizeof(crncystr_buf),
                             "%c%s", (pos == CSP_PRECEDES) ? '-' : '+', sym);
                    ret = crncystr_buf;
                } else {
                    ret = "";
                }
            } else {
                ret = "";
            }
            break;

        /* RADIXCHAR/THOUSEP: decimal point and thousands separator
         * from the AmigaOS locale's numeric formatting fields.
         */
        case RADIXCHAR:
            ret = (char *) __clib4->__default_locale->loc_DecimalPoint;
            break;
        case THOUSEP:
            ret = (char *) __clib4->__default_locale->loc_GroupSeparator;
            break;

        /* ERA and ALT_DIGITS: AmigaOS does not have era-based calendars,
         * return empty strings per POSIX (no era = empty).
         */
        case ERA:
        case ERA_D_FMT:
        case ERA_D_T_FMT:
        case ERA_T_FMT:
        case ALT_DIGITS:
            ret = "";
            break;

        default:
            ret = "";
    }

    RETURN(ret);
    return (char *) ret;
}
