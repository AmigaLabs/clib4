/*
 * $Id: stdlib_timerbase.h,v 1.0 2023-05-17 10:06:25 clib2devs Exp $
*/

#ifndef _STDLIB_TIMERBASE_H
#define _STDLIB_TIMERBASE_H

/****************************************************************************/

#ifndef __NOLIBBASE__
#define __NOLIBBASE__
#endif /* __NOLIBBASE__ */

#ifndef __NOGLOBALIFACE__
#define __NOGLOBALIFACE__
#endif /* __NOGLOBALIFACE__ */

#include <proto/timer.h>

/****************************************************************************/

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#define DECLARE_TIMERBASE() \
	struct Library *		UNUSED	TimerBase	= __CLIB2->__TimerBase; \
	struct TimerIFace *			    ITimer		= __CLIB2->__ITimer

#endif /* _STDLIB_TIMERBASE_H */
