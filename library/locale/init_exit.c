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

struct Library *NOCOMMON __LocaleBase;
struct LocaleIFace *NOCOMMON __ILocale;

struct Library *NOCOMMON __DiskfontBase;
struct DiskfontIFace *NOCOMMON __IDiskfont;

struct Locale *NOCOMMON __default_locale;
struct Locale *NOCOMMON __locale_table[NUM_LOCALES];

char NOCOMMON __locale_name_table[NUM_LOCALES][MAX_LOCALE_NAME_LEN];


void __close_all_locales(void)
{
	__locale_lock();

	if (__LocaleBase != NULL)
	{
		DECLARE_LOCALEBASE();

		int i;

		for (i = 0; i < NUM_LOCALES; i++)
		{
			if (i == LC_ALL)
				continue;

			if (__locale_table[i] != NULL)
			{
				if (__locale_table[i] != __locale_table[LC_ALL])
					CloseLocale(__locale_table[i]);

				__locale_table[i] = NULL;
			}
		}

		CloseLocale(__locale_table[LC_ALL]);
		__locale_table[LC_ALL] = NULL;
	}

	__locale_unlock();
}

void __locale_exit(void)
{
	ENTER();

	__locale_lock();

	if (__LocaleBase != NULL)
	{
		DECLARE_LOCALEBASE();

		__close_all_locales();

		if (__default_locale != NULL)
		{
			CloseLocale(__default_locale);
			__default_locale = NULL;
		}
		if (__ILocale != NULL)
		{
			DropInterface((struct Interface *)__ILocale);
			__ILocale = NULL;
		}

		CloseLibrary(__LocaleBase);
		__LocaleBase = NULL;
	}

    if (__IDiskfont != NULL) {
        DropInterface((struct Interface *)__IDiskfont);
        __IDiskfont = NULL;
    }

    if (__DiskfontBase != NULL) {
        CloseLibrary(__DiskfontBase);
        __DiskfontBase = NULL;
    }

    __locale_unlock();

	LEAVE();
}

int __locale_init(void)
{
	int result = ERROR;

	ENTER();

	__locale_lock();

	if (__LocaleBase == NULL)
	{
		__LocaleBase = OpenLibrary("locale.library", 38);
		if (__LocaleBase != NULL)
		{
			__ILocale = (struct LocaleIFace *)GetInterface(__LocaleBase, "main", 1, 0);
			if (__ILocale == NULL)
			{
				CloseLibrary(__LocaleBase);
				__LocaleBase = NULL;
			}
            __DiskfontBase = OpenLibrary("diskfont.library", 50);
            if (__DiskfontBase) {
                __IDiskfont = (struct DiskfontIFace *) GetInterface(__DiskfontBase, "main", 1, NULL);
                if (!__IDiskfont) {
                    DropInterface((struct Interface *)__ILocale);

                    CloseLibrary(__LocaleBase);
                    __LocaleBase = NULL;

                    CloseLibrary(__DiskfontBase);
                    __DiskfontBase = NULL;
                }
            }
		}
	}

	if (__LocaleBase != NULL && __default_locale == NULL)
	{
		DECLARE_LOCALEBASE();

		__default_locale = OpenLocale(NULL);
	}

	if (__default_locale != NULL)
	{
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
        strcpy(__locale_name_table[i], "C-UTF-8");
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
