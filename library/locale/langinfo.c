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

char *
nl_langinfo(nl_item item) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;
    DECLARE_LOCALEBASE_R(__clib4);
    DECLARE_FONTBASE_R(__clib4);

    const char *ret = NULL;
    char *cs;
    static char *cset = NULL;

    switch (item) {
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
        case AM_STR:
            ret = (char *) "AM"; // hardcoded
            break;
        case PM_STR:
            ret = (char *) "PM"; // hardcoded;
            break;
        case RADIXCHAR:
            ret = (char *) __clib4->__default_locale->loc_DecimalPoint;
            break;
        case THOUSEP:
            ret = (char *) __clib4->__default_locale->loc_GroupSeparator;
            break;
        default:
            ret = "";
    }

    RETURN(ret);
    return (char *) ret;
}
