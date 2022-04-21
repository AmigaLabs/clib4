/*
 * $Id: systeminfo_sysinfo.c,v 1.6 2010-08-20 15:33:36 clib2devs Exp $
*/

#include <sys/systeminfo.h>
#include <sys/utsname.h>

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

long 
sysinfo(int cmd, char *buf, long buflen)
{
	struct utsname info;
	long ret = -1;

	ENTER();

    SHOWVALUE(cmd);
    SHOWSTRING(buf);
    SHOWVALUE(buflen);

    if (buf == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	uname(&info); /* Most of the keys use this. */

	switch (cmd)
	{
	case SI_SYSNAME:
		ret = strlcpy(buf, info.sysname, buflen);
		break;
	case SI_HOSTNAME:
		ret = strlcpy(buf, info.nodename, buflen);
		break;
	case SI_SET_HOSTNAME:
		__set_errno(EPERM);
		goto out;
	case SI_RELEASE:
		ret = strlcpy(buf, info.release, buflen);
		break;
	case SI_VERSION:
		ret = strlcpy(buf, info.version, buflen);
		break;
	case SI_MACHINE:
		ret = strlcpy(buf, "amiga", buflen);
		break;
	case SI_ARCHITECTURE:
		ret = strlcpy(buf, info.machine, buflen);
		break;
	case SI_ISALIST:
	{
		/* Solaris uses "ppc" as the isa name, but gcc uses powerpc. "ppc+altivec" follows the
				convention of specifying isa+ext from Solaris on SPARC machines. This way of implementing
				SI_ISALIST seemed like a good idea at the time. */
		const char *s;
		uint32 vecu;

		GetCPUInfoTags(GCIT_VectorUnit, &vecu, TAG_DONE);
		if (vecu == VECTORTYPE_ALTIVEC || vecu == 2 /* VECTORTYPE_VMX == 2 */) /* AltiVec and VMX are the same. */
			s = "ppc+altivec ppc common";
		else
			s = "ppc common";
		ret = strlcpy(buf, s, buflen);
	}
	break;
	case SI_PLATFORM: /* TODO: Figure out a good way of checking this. */
		ret = strlcpy(buf, "Amiga,Unknown", buflen);
		break;
	case SI_HW_PROVIDER: /* TODO: Figure out a good way of checking this. */
		ret = strlcpy(buf, "Unknown", buflen);
		break;
	case SI_HW_SERIAL:
		ret = snprintf(buf, buflen, "%lu", gethostid());
		break;
	default:
		__set_errno(EINVAL);
		goto out;
	}

out:

	RETURN(ret);
	return (ret);
}
