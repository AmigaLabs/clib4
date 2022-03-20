/*
 * $Id: stdlib_localebase.h,v 1.2 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_LOCALEBASE_H
#define _STDLIB_LOCALEBASE_H

/****************************************************************************/

#ifndef __NOLIBBASE__
#define __NOLIBBASE__
#endif /* __NOLIBBASE__ */

#ifndef __NOGLOBALIFACE__
#define __NOGLOBALIFACE__
#endif /* __NOGLOBALIFACE__ */

#include <proto/locale.h>
#include <diskfont/diskfonttag.h>
#include <proto/diskfont.h>

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */


extern struct Library * NOCOMMON __LocaleBase;
extern struct LocaleIFace NOCOMMON * __ILocale;

extern struct Library *NOCOMMON __DiskfontBase;
extern struct DiskfontIFace *NOCOMMON __IDiskfont;

#define DECLARE_LOCALEBASE() \
	struct Library *		UNUSED	LocaleBase	= __LocaleBase; \
	struct LocaleIFace *			ILocale		= __ILocale

#define DECLARE_FONTBASE() \
	struct Library *		UNUSED	DiskfontBase = __DiskfontBase; \
	struct DiskfontIFace *			IDiskfont    = __IDiskfont


#endif /* _STDLIB_LOCALEBASE_H */
