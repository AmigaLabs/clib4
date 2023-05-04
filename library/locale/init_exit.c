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

void __close_all_locales(void) {
	__locale_lock();

	if (__CLIB2->__LocaleBase != NULL) {
		DECLARE_LOCALEBASE();

		int i;

		for (i = 0; i < NUM_LOCALES; i++) {
			if (i == LC_ALL)
				continue;

			if (__CLIB2->__locale_table[i] != NULL) {
				if (__CLIB2->__locale_table[i] != __CLIB2->__locale_table[LC_ALL])
					CloseLocale(__CLIB2->__locale_table[i]);

                __CLIB2->__locale_table[i] = NULL;
			}
		}

		CloseLocale(__CLIB2->__locale_table[LC_ALL]);
        __CLIB2->__locale_table[LC_ALL] = NULL;
	}

	__locale_unlock();
}

void __locale_exit(void) {
	ENTER();

	__locale_lock();

	if (__CLIB2->__LocaleBase != NULL) {
		DECLARE_LOCALEBASE();

		__close_all_locales();

		if (__CLIB2->__default_locale != NULL) {
			CloseLocale(__CLIB2->__default_locale);
            __CLIB2->__default_locale = NULL;
		}
		if (__CLIB2->__ILocale != NULL) {
			DropInterface((struct Interface *) __CLIB2->__ILocale);
            __CLIB2->__ILocale = NULL;
		}

		CloseLibrary(__CLIB2->__LocaleBase);
        __CLIB2->__LocaleBase = NULL;
	}

    if (__CLIB2->__IDiskfont != NULL) {
        DropInterface((struct Interface *) __CLIB2->__IDiskfont);
        __CLIB2->__IDiskfont = NULL;
    }

    if (__CLIB2->__DiskfontBase != NULL) {
        CloseLibrary(__CLIB2->__DiskfontBase);
        __CLIB2->__DiskfontBase = NULL;
    }

    __locale_unlock();

	LEAVE();
}

int __locale_init(void)
{
	int result = ERROR;

	ENTER();

	__locale_lock();

	if (__CLIB2->__LocaleBase == NULL) {
        __CLIB2->__LocaleBase = OpenLibrary("locale.library", 52);
		if (__CLIB2->__LocaleBase != NULL) {
            __CLIB2->__ILocale = (struct LocaleIFace *)GetInterface(__CLIB2->__LocaleBase, "main", 1, 0);
			if (__CLIB2->__ILocale == NULL) {
				CloseLibrary(__CLIB2->__LocaleBase);
                __CLIB2->__LocaleBase = NULL;
			}
            __CLIB2->__DiskfontBase = OpenLibrary("diskfont.library", 52);
            if (__CLIB2->__DiskfontBase) {
                __CLIB2->__IDiskfont = (struct DiskfontIFace *) GetInterface(__CLIB2->__DiskfontBase, "main", 1, NULL);
                if (!__CLIB2->__IDiskfont) {
                    DropInterface((struct Interface *) __CLIB2->__ILocale);

                    CloseLibrary(__CLIB2->__LocaleBase);
                    __CLIB2->__LocaleBase = NULL;

                    CloseLibrary(__CLIB2->__DiskfontBase);
                    __CLIB2->__DiskfontBase = NULL;
                }
            }
		}
	}

	if (__CLIB2->__LocaleBase != NULL && __CLIB2->__default_locale == NULL) {
		DECLARE_LOCALEBASE();

        __CLIB2->__default_locale = OpenLocale(NULL);
	}

	if (__CLIB2->__default_locale != NULL) {
		result = OK;
	}

	__locale_unlock();

	RETURN(result);
	return (result);
}

static struct SignalSemaphore * locale_lock;

void __locale_lock(void)
{
	if (locale_lock != NULL)
		ObtainSemaphore(locale_lock);
}

void __locale_unlock(void)
{
	if (locale_lock != NULL)
		ReleaseSemaphore(locale_lock);
}

CLIB_DESTRUCTOR(locale_exit)
{
	ENTER();

	__locale_exit();

	__delete_semaphore(locale_lock);
	locale_lock = NULL;

	LEAVE();
}

CLIB_CONSTRUCTOR(locale_init)
{
	BOOL success = FALSE;
	int i;

	ENTER();

	locale_lock = __create_semaphore();
	if (locale_lock == NULL)
		goto out;

	for (i = 0; i < NUM_LOCALES; i++) {
        strcpy(__CLIB2->__locale_name_table[i], "C-UTF-8");
    }

	if (__open_locale)
		__locale_init();

	success = TRUE;

out:

	SHOWVALUE(success);
	LEAVE();

	if (success)
		CONSTRUCTOR_SUCCEED();
	else
		CONSTRUCTOR_FAIL();
}
