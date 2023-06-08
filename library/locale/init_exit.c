/*
 * $Id: locale_init_exit.c,v 1.15 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include <proto/diskfont.h>

void __close_all_locales(struct _clib2 *__clib2) {
	__locale_lock(__clib2);

	if (__clib2->__LocaleBase != NULL) {
		DECLARE_LOCALEBASE();

		int i;

		for (i = 0; i < NUM_LOCALES; i++) {
			if (i == LC_ALL)
				continue;

			if (__clib2->__locale_table[i] != NULL) {
				if (__clib2->__locale_table[i] != __clib2->__locale_table[LC_ALL])
					CloseLocale(__clib2->__locale_table[i]);

                __clib2->__locale_table[i] = NULL;
			}
		}

		CloseLocale(__clib2->__locale_table[LC_ALL]);
        __clib2->__locale_table[LC_ALL] = NULL;
	}

	__locale_unlock(__clib2);
}

void __locale_exit(struct _clib2 *__clib2) {
	ENTER();

	__locale_lock(__clib2);

	if (__clib2->__LocaleBase != NULL) {
		DECLARE_LOCALEBASE();

		__close_all_locales(__clib2);

		if (__clib2->__default_locale != NULL) {
			CloseLocale(__clib2->__default_locale);
            __clib2->__default_locale = NULL;
		}
		if (__clib2->__ILocale != NULL) {
			DropInterface((struct Interface *) __clib2->__ILocale);
            __clib2->__ILocale = NULL;
		}

		CloseLibrary(__clib2->__LocaleBase);
        __clib2->__LocaleBase = NULL;
	}

    if (__clib2->__IDiskfont != NULL) {
        DropInterface((struct Interface *) __clib2->__IDiskfont);
        __clib2->__IDiskfont = NULL;
    }

    if (__clib2->__DiskfontBase != NULL) {
        CloseLibrary(__clib2->__DiskfontBase);
        __clib2->__DiskfontBase = NULL;
    }

    __locale_unlock(__clib2);

	LEAVE();
}

int __locale_init(struct _clib2 *__clib2) {
	int result = ERROR;

	ENTER();

	__locale_lock(__clib2);

	if (__clib2->__LocaleBase == NULL) {
        __clib2->__LocaleBase = OpenLibrary("locale.library", 52);
		if (__clib2->__LocaleBase != NULL) {
            __clib2->__ILocale = (struct LocaleIFace *)GetInterface(__clib2->__LocaleBase, "main", 1, 0);
			if (__clib2->__ILocale == NULL) {
				CloseLibrary(__clib2->__LocaleBase);
                __clib2->__LocaleBase = NULL;
			}
            __clib2->__DiskfontBase = OpenLibrary("diskfont.library", 52);
            if (__clib2->__DiskfontBase) {
                __clib2->__IDiskfont = (struct DiskfontIFace *) GetInterface(__clib2->__DiskfontBase, "main", 1, NULL);
                if (!__clib2->__IDiskfont) {
                    DropInterface((struct Interface *) __clib2->__ILocale);

                    CloseLibrary(__clib2->__LocaleBase);
                    __clib2->__LocaleBase = NULL;

                    CloseLibrary(__clib2->__DiskfontBase);
                    __clib2->__DiskfontBase = NULL;
                }
            }
		}
	}

	if (__clib2->__LocaleBase != NULL && __clib2->__default_locale == NULL) {
		DECLARE_LOCALEBASE();

        __clib2->__default_locale = OpenLocale(NULL);
	}

	if (__clib2->__default_locale != NULL) {
		result = OK;
	}

	__locale_unlock(__clib2);

	RETURN(result);
	return (result);
}

void __locale_lock(struct _clib2 *__clib2) {
	if (__clib2->locale_lock != NULL)
		ObtainSemaphore(__clib2->locale_lock);
}

void __locale_unlock(struct _clib2 *__clib2) {
	if (__clib2->locale_lock != NULL)
		ReleaseSemaphore(__clib2->locale_lock);
}

CLIB_DESTRUCTOR(locale_exit) {
	ENTER();
    struct _clib2 *__clib2 = __CLIB2;

	__locale_exit(__clib2);

	__delete_semaphore(__clib2->locale_lock);
    __clib2->locale_lock = NULL;

    __delete_semaphore(__clib2->gettext_lock);
    __clib2->gettext_lock = NULL;

    LEAVE();
}

CLIB_CONSTRUCTOR(locale_init) {
	BOOL success = FALSE;
	int i;
    struct _clib2 *__clib2 = __CLIB2;

	ENTER();

    __clib2->locale_lock = __create_semaphore();
	if (__clib2->locale_lock == NULL) {
        goto out;
    }

    __clib2->gettext_lock = __create_semaphore();
    if (__clib2->gettext_lock == NULL) {
        goto out;
    }

	for (i = 0; i < NUM_LOCALES; i++) {
        strcpy(__clib2->__locale_name_table[i], "C-UTF-8");
    }

    strncpy(__clib2->gettext_domain, "messages", NAME_MAX);

    __locale_init(__clib2);

	success = TRUE;

out:

	SHOWVALUE(success);
	LEAVE();

	if (success)
		CONSTRUCTOR_SUCCEED();
	else
        CONSTRUCTOR_FAIL();
}