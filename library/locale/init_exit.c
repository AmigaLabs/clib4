/*
 * $Id: locale_init_exit.c,v 1.15 2006-01-08 12:04:23 clib4devs Exp $
*/

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include <proto/diskfont.h>

void __close_all_locales(struct _clib4 *__clib4) {
	__locale_lock(__clib4);

	if (__clib4->__LocaleBase != NULL) {
        DECLARE_LOCALEBASE_R(__clib4);

		int i;

		for (i = 0; i < NUM_LOCALES; i++) {
			if (i == LC_ALL)
				continue;

			if (__clib4->__locale_table[i] != NULL) {
				if (__clib4->__locale_table[i] != __clib4->__locale_table[LC_ALL])
					CloseLocale(__clib4->__locale_table[i]);

                __clib4->__locale_table[i] = NULL;
			}
		}

		CloseLocale(__clib4->__locale_table[LC_ALL]);
        __clib4->__locale_table[LC_ALL] = NULL;
	}

	__locale_unlock(__clib4);
}

void __locale_exit(struct _clib4 *__clib4) {
	ENTER();

	__locale_lock(__clib4);

	if (__clib4->__LocaleBase != NULL) {
        DECLARE_LOCALEBASE_R(__clib4);

		__close_all_locales(__clib4);

		if (__clib4->__default_locale != NULL) {
			CloseLocale(__clib4->__default_locale);
            __clib4->__default_locale = NULL;
		}
		if (__clib4->__ILocale != NULL) {
			DropInterface((struct Interface *) __clib4->__ILocale);
            __clib4->__ILocale = NULL;
		}

		CloseLibrary(__clib4->__LocaleBase);
        __clib4->__LocaleBase = NULL;
	}

    if (__clib4->__IDiskfont != NULL) {
        DropInterface((struct Interface *) __clib4->__IDiskfont);
        __clib4->__IDiskfont = NULL;
    }

    if (__clib4->__DiskfontBase != NULL) {
        CloseLibrary(__clib4->__DiskfontBase);
        __clib4->__DiskfontBase = NULL;
    }

    __locale_unlock(__clib4);

	LEAVE();
}

int __locale_init(struct _clib4 *__clib4) {
	int result = ERROR;

	ENTER();

	__locale_lock(__clib4);

	if (__clib4->__LocaleBase == NULL) {
        __clib4->__LocaleBase = OpenLibrary("locale.library", 52);
		if (__clib4->__LocaleBase != NULL) {
            __clib4->__ILocale = (struct LocaleIFace *)GetInterface(__clib4->__LocaleBase, "main", 1, 0);
			if (__clib4->__ILocale == NULL) {
				CloseLibrary(__clib4->__LocaleBase);
                __clib4->__LocaleBase = NULL;
			}
            __clib4->__DiskfontBase = OpenLibrary("diskfont.library", 52);
            if (__clib4->__DiskfontBase) {
                __clib4->__IDiskfont = (struct DiskfontIFace *) GetInterface(__clib4->__DiskfontBase, "main", 1, NULL);
                if (!__clib4->__IDiskfont) {
                    DropInterface((struct Interface *) __clib4->__ILocale);

                    CloseLibrary(__clib4->__LocaleBase);
                    __clib4->__LocaleBase = NULL;

                    CloseLibrary(__clib4->__DiskfontBase);
                    __clib4->__DiskfontBase = NULL;
                }
            }
		}
	}

	if (__clib4->__LocaleBase != NULL && __clib4->__default_locale == NULL) {
        DECLARE_LOCALEBASE_R(__clib4);

        __clib4->__default_locale = OpenLocale(NULL);
	}

	if (__clib4->__default_locale != NULL) {
		result = OK;
	}

	__locale_unlock(__clib4);

	RETURN(result);
	return (result);
}

void __locale_lock(struct _clib4 *__clib4) {
	if (__clib4->locale_lock != NULL)
		ObtainSemaphore(__clib4->locale_lock);
}

void __locale_unlock(struct _clib4 *__clib4) {
	if (__clib4->locale_lock != NULL)
		ReleaseSemaphore(__clib4->locale_lock);
}

CLIB_DESTRUCTOR(locale_exit) {
	ENTER();
    struct _clib4 *__clib4 = __CLIB4;

	__locale_exit(__clib4);

	__delete_semaphore(__clib4->locale_lock);
    __clib4->locale_lock = NULL;

    __delete_semaphore(__clib4->gettext_lock);
    __clib4->gettext_lock = NULL;

    LEAVE();
}

CLIB_CONSTRUCTOR(locale_init) {
	BOOL success = FALSE;
	int i;
    struct _clib4 *__clib4 = __CLIB4;

	ENTER();

    __clib4->locale_lock = __create_semaphore();
	if (__clib4->locale_lock == NULL) {
        goto out;
    }

    __clib4->gettext_lock = __create_semaphore();
    if (__clib4->gettext_lock == NULL) {
        goto out;
    }

	for (i = 0; i < NUM_LOCALES; i++) {
        strcpy(__clib4->__locale_name_table[i], "C-UTF-8");
    }

    strncpy(__clib4->gettext_domain, "messages", NAME_MAX);

    __locale_init(__clib4);

	success = TRUE;

out:

	SHOWVALUE(success);
	LEAVE();

	if (success)
		CONSTRUCTOR_SUCCEED();
	else
        CONSTRUCTOR_FAIL();
}