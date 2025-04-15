/*
 * $Id: stdlib_program_name.c,v 1.4 2025-03-23 14:09:00 clib4devs Exp $
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

void stdlib_program_name_exit() {
	ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->free_program_name && __clib4->__progname != NULL) {
		FreeVec(__clib4->__progname);
        __clib4->__progname = NULL;
	}

	LEAVE();
}

/* First constructor called by _init */
BOOL
stdlib_program_name_init() {
	BOOL success = FALSE;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

	if (__clib4->__WBenchMsg == NULL) {
		const size_t program_name_size = 256;

		/* Make a copy of the current command name string. */
        __clib4->__progname = AllocVecTags((ULONG)program_name_size, AVT_Type, MEMF_SHARED, TAG_DONE);
		if (__clib4->__progname == NULL) {
            SHOWMSG("Cannot allocate program_name_size memory. Give up");
            goto out;
        }

        __clib4->free_program_name = TRUE;

		if (CANNOT GetCliProgramName(__clib4->__progname, program_name_size)) {
            SHOWMSG("Cannot get cli program name. Most probably we are in a .library or in a program doesn't have main");
            FreeVec(__clib4->__progname);
            __clib4->free_program_name = FALSE;

            __clib4->__progname = "unknown";
        }
	}
	else {
        __clib4->__progname = (char *) __clib4->__WBenchMsg->sm_ArgList[0].wa_Name;
	}

	success = TRUE;

out:

	RETURN(success);
	return success;
}


const char *__getprogname(void) {
    return __CLIB4->__progname;
}