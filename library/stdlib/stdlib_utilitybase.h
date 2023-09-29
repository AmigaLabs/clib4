/*
 * $Id: stdlib_utilitybase.h,v 1.2 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_UTILITYBASE_H
#define _STDLIB_UTILITYBASE_H

#ifndef __NOLIBBASE__
#define __NOLIBBASE__
#endif /* __NOLIBBASE__ */

#ifndef __NOGLOBALIFACE__
#define __NOGLOBALIFACE__
#endif /* __NOGLOBALIFACE__ */

#include <proto/utility.h>

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

extern struct Library* __UtilityBase;
extern struct UtilityIFace* __IUtility;

#define DECLARE_UTILITYBASE() \
	struct Library *		UNUSED	UtilityBase	= __UtilityBase; \
	struct UtilityIFace *			IUtility	= __IUtility


#endif /* _STDLIB_UTILITYBASE_H */
