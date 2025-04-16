/*
 * $Id: stdlib_debug.h,v 1.0 2025-04-15 10:06:25 clib4devs Exp $
*/

#ifndef _STDLIB_DEBUGBASE_H
#define _STDLIB_DEBUGBASE_H

/****************************************************************************/

#ifndef __NOLIBBASE__
#define __NOLIBBASE__
#endif /* __NOLIBBASE__ */

#ifndef __NOGLOBALIFACE__
#define __NOGLOBALIFACE__
#endif /* __NOGLOBALIFACE__ */

#include <proto/exec.h>

/****************************************************************************/

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#define DECLARE_DEBUGBASE() \
	struct DebugIFace *IDebug = __CLIB4->__IDebug

#define DECLARE_DEBUGBASE_R(clib4) \
	struct DebugIFace *IDebug = clib4->__IDebug

#endif /* _STDLIB_DEBUGBASE_H */
