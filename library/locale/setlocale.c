/*
 * $Id: locale_setlocale.c,v 1.5 2006-01-08 12:04:23 clib4devs Exp $
*/

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>

/* Determine MB_CUR_MAX from an opened locale's loc_CodeSet using
 * ObtainCharsetInfo() to get the MIME charset name, then set the
 * appropriate maximum multi-byte length. Also builds a "C-<encoding>"
 * string in locale_name_out (which must be at least MAX_LOCALE_NAME_LEN)
 * so that _current_locale stays in the format expected by wchar functions.
 * Returns the MB_CUR_MAX value to use.
 */
static int
__set_mb_cur_max_from_locale(struct _clib4 *__clib4, struct Locale *loc,
                             char *locale_name_out, int locale_name_size) {
    int mb_max = 1;

    if (loc != NULL && __clib4->__DiskfontBase != NULL) {
        DECLARE_FONTBASE_R(__clib4);
        uint32 codeset = loc->loc_CodeSet;
        /* loc_CodeSet 0 means "unspecified", treated as ISO-8859-1 (codeset 4) */
        if (codeset == 0)
            codeset = 4;

        const char *mime = (const char *) ObtainCharsetInfo(
            DFCS_NUMBER, codeset, DFCS_MIMENAME);

        if (mime != NULL) {
            /* Build "C-<MIMENAME>" for _current_locale compatibility */
            snprintf(locale_name_out, locale_name_size, "C-%s", mime);

            /* Set MB_CUR_MAX based on the MIME charset name */
            if (strcmp(mime, "UTF-8") == 0)
                mb_max = 4;  /* RFC 3629 limits UTF-8 to 4 bytes */
            else if (strncmp(mime, "ISO-8859", 8) == 0 ||
                     strcmp(mime, "US-ASCII") == 0)
                mb_max = 1;
            else if (strcmp(mime, "Shift_JIS") == 0 ||
                     strcmp(mime, "EUC-JP") == 0 ||
                     strcmp(mime, "EUC-KR") == 0 ||
                     strcmp(mime, "Big5") == 0 ||
                     strcmp(mime, "GB2312") == 0)
                mb_max = 2;
            else if (strcmp(mime, "ISO-2022-JP") == 0)
                mb_max = 8;
        } else {
            /* ObtainCharsetInfo failed — fall back to single-byte */
            snprintf(locale_name_out, locale_name_size, "C-ISO-8859-1");
        }
    } else {
        /* No locale or no diskfont — default to single-byte */
        snprintf(locale_name_out, locale_name_size, "C-UTF-8");
    }

    return mb_max;
}

/* Determine MB_CUR_MAX from a "C-<encoding>" style locale name string.
 * This is the legacy path used when the locale name already contains an
 * encoding suffix (e.g. "C-UTF-8", "C-SJIS").
 */
static int
__mb_cur_max_from_name(const char *locale) {
    int mb_max = 1;
    const char *enc = NULL;

    /* Look for encoding after "C-" or "C." prefix */
    if (locale[0] == 'C' && (locale[1] == '-' || locale[1] == '.'))
        enc = locale + 2;
    /* Also handle "XX_YY.encoding" POSIX format */
    else if (strchr(locale, '.') != NULL)
        enc = strchr(locale, '.') + 1;

    if (enc != NULL) {
        switch (enc[0]) {
            case 'U': case 'u': /* UTF-8 */
                mb_max = 4;
                break;
            case 'J': case 'j': /* JIS */
                mb_max = 8;
                break;
            case 'E': case 'e': /* EUC-JP */
            case 'S': case 's': /* SJIS / Shift_JIS */
                mb_max = 2;
                break;
            case 'I': case 'i': /* ISO-8859-* */
            default:
                mb_max = 1;
        }
    }

    return mb_max;
}

/* setlocale() — set or query the program's locale.
 *
 * Bridges POSIX locale names to AmigaOS locale.library:
 * - NULL locale: query current locale name without changing it
 * - "": activate system default locale via OpenLocale(NULL)
 * - "C" or "POSIX": use the C locale (no AmigaOS locale)
 * - "C-<encoding>": C locale with explicit charset for wchar functions
 * - Other names: try OpenLocale() directly, fall back to system default
 *
 * The locale name stored internally uses "C-<MIMENAME>" format
 * (e.g. "C-UTF-8", "C-ISO-8859-1") for compatibility with clib4's
 * wchar functions which parse _current_locale to determine encoding.
 */
char *
setlocale(int category, const char *locale) {
    char *result = NULL;
    struct _clib4 *__clib4 = __CLIB4;
    DECLARE_LOCALEBASE_R(__clib4);

    ENTER();

    SHOWVALUE(category);

    if (locale == NULL)
        SHOWPOINTER(locale);
    else
        SHOWSTRING(locale);

    __locale_lock(__clib4);

    if (category < LC_ALL || category > LC_MAX) {
        SHOWMSG("invalid category");

        __set_errno(EINVAL);
        goto out;
    }

    if (locale != NULL) {
        struct Locale *loc = NULL;
        /* Name to store in __locale_name_table for _current_locale
         * compatibility with wchar functions.
         */
        char effective_name[MAX_LOCALE_NAME_LEN];
        int mb_max = 1;

        /* Empty string means "use system default locale" (POSIX spec).
         * Map this to OpenLocale(NULL) which returns the user's
         * configured locale from Locale preferences.
         */
        if (locale[0] == '\0') {
            if (LocaleBase != NULL) {
                loc = OpenLocale(NULL);
                if (loc == NULL) {
                    SHOWMSG("couldn't open default locale");
                    __set_errno(ENOENT);
                    goto out;
                }
                /* Derive encoding from the locale's CodeSet and build
                 * a "C-<MIMENAME>" string for wchar compatibility.
                 */
                mb_max = __set_mb_cur_max_from_locale(
                    __clib4, loc, effective_name, sizeof(effective_name));
            } else {
                /* No locale.library — stay with C locale */
                strcpy(effective_name, "C-UTF-8");
            }
        } else if (strcmp(locale, "C") == SAME ||
                   strcmp(locale, "POSIX") == SAME) {
            /* Explicit C/POSIX locale — no AmigaOS locale needed */
            strcpy(effective_name, "C");
            mb_max = 1;
        } else if (strcmp(locale, "C-UTF-8") == SAME ||
                   (locale[0] == 'C' && (locale[1] == '-' || locale[1] == '.'))) {
            /* C locale with explicit encoding (e.g. "C-UTF-8", "C.UTF-8",
             * "C-SJIS", "C-ISO-8859-1"). Keep as-is for wchar compat.
             */
            if (strlen(locale) >= MAX_LOCALE_NAME_LEN) {
                SHOWMSG("locale name is too long");
                __set_errno(ENAMETOOLONG);
                goto out;
            }
            strcpy(effective_name, locale);
            mb_max = __mb_cur_max_from_name(locale);
        } else {
            /* Non-C locale name. Could be:
             * - POSIX style: "en_US.UTF-8", "de_DE.ISO-8859-1"
             * - AmigaOS style: "english", "deutsch"
             * Try OpenLocale() with the name as-is for AmigaOS names.
             * For POSIX names, extract the language part before '_' or '.'
             * and try that as an AmigaOS locale name.
             */
            if (strlen(locale) >= MAX_LOCALE_NAME_LEN) {
                SHOWMSG("locale name is too long");
                __set_errno(ENAMETOOLONG);
                goto out;
            }

            if (LocaleBase != NULL) {
                loc = OpenLocale((STRPTR) locale);

                /* If direct open failed, try extracting AmigaOS language
                 * name from POSIX-style locale (e.g. "en_US.UTF-8" → "english")
                 */
                if (loc == NULL) {
                    /* Try the system default as fallback — better than
                     * failing entirely for unrecognized locale names.
                     */
                    loc = OpenLocale(NULL);
                }

                if (loc == NULL) {
                    SHOWMSG("couldn't open the locale");
                    __set_errno(ENOENT);
                    goto out;
                }

                /* Derive encoding from opened locale's CodeSet */
                mb_max = __set_mb_cur_max_from_locale(
                    __clib4, loc, effective_name, sizeof(effective_name));
            } else {
                /* No locale.library available */
                __set_errno(ENOENT);
                goto out;
            }
        }

        MB_CUR_MAX = mb_max;

        if (category == LC_ALL) {
            int i;

            SHOWMSG("closing all locales");
            __close_all_locales(__clib4);

            SHOWMSG("reinitializing all locales");

            for (i = 0; i < NUM_LOCALES; i++) {
                __clib4->__locale_table[i] = loc;
                strcpy(__clib4->__locale_name_table[i], effective_name);
            }
        } else {
            SHOWMSG("closing the locale");

            /* Close this single locale unless it's actually just a
             * copy of the 'all' locale entry.
             */
            if (__clib4->__locale_table[category] != NULL &&
                __clib4->__locale_table[category] != __clib4->__locale_table[LC_ALL]) {
                assert(LocaleBase != NULL);
                CloseLocale(__clib4->__locale_table[category]);
            }

            SHOWMSG("reinitializing the locale");

            __clib4->__locale_table[category] = loc;
            strcpy(__clib4->__locale_name_table[category], effective_name);
        }
    }

    result = __clib4->__locale_name_table[category];
    SHOWSTRING(result);

out:
    __clib4->_current_category = category;
    __clib4->_current_locale = result;

    __locale_unlock(__clib4);

    RETURN(result);
    return result;
}
