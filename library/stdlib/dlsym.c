/*
 * $Id: stdlib_dlsym.c,v 1.2 2010-08-21 11:37:03 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <dlfcn.h>
#include <libraries/elf.h>
#include <proto/elf.h>

void *
dlsym(void *handle, const char *symbol_name)
{
	void *result = NULL;

    ENTER();
    SHOWPOINTER(handle);
    SHOWSTRING(symbol_name);

    if (__global_clib2->__dl_elf_handle != NULL)
	{
		struct ElfIFace *IElf = __global_clib2->IElf;
		APTR symbol_data = NULL;
		Elf32_Error error;

		error = DLSym(__global_clib2->__dl_elf_handle, handle, symbol_name, &symbol_data);
		if (error != ELF32_NO_ERROR)
		{
			__global_clib2->__elf_error_code = error;
			goto out;
		}
		result = symbol_data;
	}
	else {
		__set_errno(ENOSYS);
	}
	
out:
    RETURN(result);
	return (result);
}
