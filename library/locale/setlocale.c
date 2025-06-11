/*
 * $Id: locale_setlocale.c,v 1.5 2006-01-08 12:04:23 clib4devs Exp $
*/

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

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

    if (locale != NULL && locale[0] != '\0') {
        struct Locale *loc = NULL;

        /* We have to keep the locale name for later reference.
         * On the Amiga this will be a path and file name so it
         * can become rather long. But we can't store an arbitrarily
         * long name either.
         *
         * ZZZ change this to dynamic allocation of the locale name.
         */
        if (strlen(locale) >= MAX_LOCALE_NAME_LEN) {
            SHOWMSG("locale name is too long");
            result = (char *)"C-UTF-8";

            __set_errno(ENAMETOOLONG);
            goto out;
        }

        /* Unless we are switching to the "C" locale,
         * try to open a locale if we managed to open
         * locale.library before.
         */
        if (LocaleBase != NULL) {
            if (strcmp(locale, "C") != SAME && strcmp(locale, "C-UTF-8") != SAME) {
                SHOWMSG("this is not the 'C' locale");
                /* The empty string stands for the default locale. */
                if (locale[0] == '\0')
                    loc = OpenLocale(NULL);
                else
                    loc = OpenLocale((STRPTR) locale);

                /* Before bailing out set the MB_CUR_MAX to the right value
                 * so we can use wide chars correctly
                 * We accept only C locales with a different encodings like C-UTF8 or C.UTF8
                 */
                MB_CUR_MAX = 1;
                if (locale[1] == '-' || locale[1] == '.') {
                    switch (locale[2]) {
                        case 'U':
                        case 'u':
                            MB_CUR_MAX = 6;
                            break;
                        case 'J':
                        case 'j':
                            MB_CUR_MAX = 8;
                            break;
                        case 'E':
                        case 'e':
                            MB_CUR_MAX = 2;
                            break;
                        case 'S':
                        case 's':
                            MB_CUR_MAX = 2;
                            break;
                        case 'I':
                        case 'i':
                        default:
                            MB_CUR_MAX = 1;
                    }
                }
                /*
                else if (strchr(locale, '.') != NULL) {
                    int index = strchr(locale, '.') - locale + 1;
                    switch (locale[index]) {
                        case 'U':
                        case 'u':
                            MB_CUR_MAX = 6;
                            break;
                        case 'J':
                        case 'j':
                            MB_CUR_MAX = 8;
                            break;
                        case 'E':
                        case 'e':
                            MB_CUR_MAX = 2;
                            break;
                        case 'S':
                        case 's':
                            MB_CUR_MAX = 2;
                            break;
                        case 'I':
                        case 'i':
                        default:
                            MB_CUR_MAX = 1;
                    }
                }
                */

                if (loc == NULL) {
                    SHOWMSG("couldn't open the locale");
                    result = NULL;

                    __set_errno(ENOENT);
                    goto out;
                }
            }
        }

        if (category == LC_ALL) {
            int i;

            SHOWMSG("closing all locales");

            /* We have to replace all locales. We
             * start by closing them all.
             */
            __close_all_locales(__clib4);

            SHOWMSG("reinitializing all locales");

            /* And this puts the new locale into all table entries. */
            for (i = 0; i < NUM_LOCALES; i++) {
                __clib4->__locale_table[i] = loc;
                if (locale[0] != '\0')
                    strcpy(__clib4->__locale_name_table[i], locale);
                else
                    strcpy(__clib4->__locale_name_table[i], "C-UTF-8");
            }

            if (strcmp(locale, "C") == SAME || strcmp(locale, "C-UTF-8") == SAME) {
                MB_CUR_MAX = 1;
            } else {
                MB_CUR_MAX = 1;
                if (locale[1] == '-' || locale[1] == '.') {
                    switch (locale[2]) {
                        case 'U':
                        case 'u':
                            MB_CUR_MAX = 6;
                            break;
                        case 'J':
                        case 'j':
                            MB_CUR_MAX = 8;
                            break;
                        case 'E':
                        case 'e':
                            MB_CUR_MAX = 2;
                            break;
                        case 'S':
                        case 's':
                            MB_CUR_MAX = 2;
                            break;
                        case 'I':
                        case 'i':
                        default:
                            MB_CUR_MAX = 1;
                    }
                }
                /*
                else if (strchr(locale, '.') != NULL) {
                    int index = strchr(locale, '.') - locale + 1;
                    switch (locale[index]) {
                        case 'U':
                        case 'u':
                            MB_CUR_MAX = 6;
                            break;
                        case 'J':
                        case 'j':
                            MB_CUR_MAX = 8;
                            break;
                        case 'E':
                        case 'e':
                            MB_CUR_MAX = 2;
                            break;
                        case 'S':
                        case 's':
                            MB_CUR_MAX = 2;
                            break;
                        case 'I':
                        case 'i':
                        default:
                            MB_CUR_MAX = 1;
                    }
                }
                */

                else {
                    SHOWMSG("couldn't open the locale");
                    result = (char *)locale;

                    __set_errno(ENOENT);
                    goto out;
                }
            }
        } else {
            SHOWMSG("closing the locale");

            /* Close this single locale unless it's actually just a
             * copy of the 'all' locale entry.
             */
            if (__clib4->__locale_table[category] != NULL && __clib4->__locale_table[category] != __clib4->__locale_table[LC_ALL]) {
                assert(LocaleBase != NULL);
                CloseLocale(__clib4->__locale_table[category]);
            }

            SHOWMSG("reinitializing the locale");

            __clib4->__locale_table[category] = loc;
            if (locale[0] != '\0') {
                strcpy(__clib4->__locale_name_table[category], locale);
            }
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