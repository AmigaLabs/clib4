/*
 * $Id: stdlib_dlopen.c,v 1.2 2010-08-21 11:37:03 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

#include <dlfcn.h>
#include <libraries/elf.h>
#include <proto/elf.h>

void *dlopen(const char *path_name, int mode)
{
	void *result = NULL;

	if (path_name == NULL || path_name[0] == '\0') {
		__set_errno(ENOENT);
		goto out;
	}

#if defined(UNIX_PATH_SEMANTICS)
	struct name_translation_info path_name_nti;
	if (__global_clib2 != NULL && __global_clib2->__unix_path_semantics)
	{
		if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
			goto out;

		if (path_name_nti.is_root)
		{
			__set_errno(EACCES);
			goto out;
		}
	}
#endif /* UNIX_PATH_SEMANTICS */

	if (__global_clib2->__dl_elf_handle != NULL)
	{
		struct ElfIFace *IElf = __global_clib2->IElf;
		uint32 flags = 0;

		if (mode & RTLD_LOCAL)
			flags = ELF32_RTLD_LOCAL;

		if (mode & RTLD_GLOBAL)
			flags = ELF32_RTLD_GLOBAL;

		result = DLOpen(__global_clib2->__dl_elf_handle, path_name, flags);
	}
	else
	{
		__set_errno(ENOSYS);
	}
out:

	return (result);
}
