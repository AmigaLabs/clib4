/*
 * $Id: stdlib_program_name.c,v 1.3 2008-09-30 14:09:00 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

static BOOL free_program_name;
char *NOCOMMON __program_name;

STDLIB_DESTRUCTOR(stdlib_program_name_exit) {
	ENTER();

	if (free_program_name && __program_name != NULL)
	{
		FreeVec(__program_name);
		__program_name = NULL;
	}

	LEAVE();
}
/* First constructor called by _init */
STDLIB_CONSTRUCTOR(stdlib_program_name_init) {
	BOOL success = FALSE;

	ENTER();

	if (__WBenchMsg == NULL) {
		const size_t program_name_size = 256;

		/* Make a copy of the current command name string. */
		__program_name = AllocVecTags((ULONG)program_name_size, AVT_Type, MEMF_SHARED, TAG_DONE);
		if (__program_name == NULL)
			goto out;

		free_program_name = TRUE;

		if (CANNOT GetCliProgramName(__program_name, program_name_size))
			goto out;
	}
	else {
		__program_name = (char *)__WBenchMsg->sm_ArgList[0].wa_Name;
	}

	success = TRUE;

out:

	SHOWVALUE(success);
	LEAVE();

	if (success)
		CONSTRUCTOR_SUCCEED();
	else
		CONSTRUCTOR_FAIL();
}
