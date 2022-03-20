/*
 * $Id: utsname_uname.c,v 1.8 2010-08-20 15:33:36 clib2devs Exp $
*/

#include <sys/utsname.h>

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#define OSNAME "AmigaOS"
#define ARCH "ppc"

int 
uname(struct utsname *info)
{
	struct Library *VersionBase;
	int Version, Revision;
	const char *version_string;
	int result = ERROR;

	ENTER();

	SHOWPOINTER(info);

	if (info == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	strlcpy(info->sysname, OSNAME, sizeof(info->sysname));
	__gethostname((STRPTR)info->nodename, sizeof(info->nodename));

	VersionBase = OpenLibrary("version.library", 0L);
	if (VersionBase != NULL)
	{
		Version = VersionBase->lib_Version;
		Revision = VersionBase->lib_Revision;

		CloseLibrary(VersionBase);
	}
	else
	{
		Version = 0;
		Revision = 0;
	}

	snprintf(info->release, sizeof(info->release), "%d.%d", Version, Revision);

	/*
	 * This is mostly a stab in the dark. Is there any "official" way of finding out the OS version?
	 * Also, this could be more detailed e.g. "3.9-BB2" instead of just "3.9".
	 * For the curious:
	 *
	 *  V.R		OS Version
	 *
	 * 30.x		1.0 (Never released)
	 * 31.x		1.1 (NTSC)
	 * 32.x		1.1 (PAL)
	 * 33.x		1.2
	 * 34.x		1.3
	 * 35.x		1.3 (with A2024 support)
	 * 36.x		2.0-2.02
	 * 37.x		2.04-2.05
	 * 38.x		2.1
	 * 39.x		3.0
	 * 40.x		3.1
	 * 44.2		3.5
	 * 44.4		3.5-BB1
	 * 44.5		3.5-BB2
	 * 45.1		3.9
	 * 45.2		3.9-BB1
	 * 45.3		3.9-BB2
	 */

	if (Version == 53)
		version_string = "4.1";
	else if (46 <= Version && Version <= 52)
		version_string = "4.0";
	else if (Version == 45)
		version_string = "3.9";
	else if (Version == 44)
		version_string = "3.5";
	else if (40 <= Version && Version <= 43)
		version_string = "3.1";
	else if (Version == 39)
		version_string = "3.0";
	else if (Version == 38)
		version_string = "2.1";
	else if (Version == 37)
		version_string = "2.0";
	else
		version_string = "unknown";

	strlcpy(info->version, version_string, sizeof(info->version));

	strlcpy(info->machine, ARCH, sizeof(info->machine));

	SHOWSTRING(info->sysname);
	SHOWSTRING(info->nodename);
	SHOWSTRING(info->release);
	SHOWSTRING(info->version);
	SHOWSTRING(info->machine);

	result = OK;

out:

	RETURN(result);
	return (result);
}
