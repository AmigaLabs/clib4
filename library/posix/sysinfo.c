/*
 * $Id: systeminfo_sysinfo.c,v 1.6 2010-08-20 15:33:36 obarthel Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
