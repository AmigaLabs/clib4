/*
 * $Id: usergroup_headers.h,v 1.8 2006-01-08 12:04:27 obarthel Exp $
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

#ifndef _USERGROUP_HEADERS_H
#define _USERGROUP_HEADERS_H

/****************************************************************************/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_PROFILE_H
#include "stdlib_profile.h"
#endif /* _STDLIB_PROFILE_H */

/****************************************************************************/

#ifndef _PWD_H
#include <pwd.h>
#endif /* _PWD_H */

#ifndef _GRP_H
#include <grp.h>
#endif /* _GRP_H */

/****************************************************************************/

extern struct Library * NOCOMMON __UserGroupBase;
extern struct UserGroupIFace * NOCOMMON __IUserGroup;

/****************************************************************************/

extern BOOL NOCOMMON __root_mode;

/****************************************************************************/

extern int NOCOMMON __root_uid;
extern int NOCOMMON __root_gid;
extern int NOCOMMON __root_euid;
extern int NOCOMMON __root_egid;

/****************************************************************************/

#include <interfaces/usergroup.h>

#define __ug_SetupContextTagList(name, tags) __IUserGroup->ug_SetupContextTagList(name, tags)
#define __ug_GetErr() __IUserGroup->ug_GetErr()
#define __ug_StrError(err) __IUserGroup->ug_StrError(err)
#define __getuid() __IUserGroup->getuid()
#define __geteuid() __IUserGroup->geteuid()
#define __setreuid(real, effective) __IUserGroup->setreuid(real, effective)
#define __setuid(uid) __IUserGroup->setuid(uid)
#define __getgid() __IUserGroup->getgid()
#define __getegid() __IUserGroup->getegid()
#define __setregid(real, effective) __IUserGroup->setregid(real, effective)
#define __setgid(gid) __IUserGroup->setgid(gid)
#define __getgroups(gidsetlen, gidset) __IUserGroup->getgroups(gidsetlen, gidset)
#define __setgroups(gidsetlen, gidset) __IUserGroup->setgroups(gidsetlen, gidset)
#define __initgroups(name, basegid) __IUserGroup->initgroups(name, basegid)
#define __getpwnam(login) __IUserGroup->getpwnam(login)
#define __getpwuid(uid) __IUserGroup->getpwuid(uid)
#define __setpwent() __IUserGroup->setpwent()
#define __getpwent() __IUserGroup->getpwent()
#define __endpwent() __IUserGroup->endpwent()
#define __getgrnam(name) __IUserGroup->getgrnam(name)
#define __getgrgid(gid) __IUserGroup->getgrgid(gid)
#define __setgrent() __IUserGroup->setgrent()
#define __getgrent() __IUserGroup->getgrent()
#define __endgrent() __IUserGroup->endgrent()
#define __crypt(key, set) __IUserGroup->crypt(key, set)
#define __ug_GetSalt(user, buf, size) __IUserGroup->ug_GetSalt(user, buf, size)
#define __getpass(prompt) __IUserGroup->getpass(prompt)
#define __umask(mask) __IUserGroup->umask(mask)
#define __getumask() __IUserGroup->getumask()
#define __setsid() __IUserGroup->setsid()
#define __getpgrp() __IUserGroup->getpgrp()
#define __getlogin() __IUserGroup->getlogin()
#define __setlogin(name) __IUserGroup->setlogin(name)
#define __setutent() __IUserGroup->setutent()
#define __getutent() __IUserGroup->getutent()
#define __endutent() __IUserGroup->endutent()
#define __getlastlog(uid) __IUserGroup->getlastlog(uid)
#define __setlastlog(uid, name, host) __IUserGroup->setlastlog(uid, name, host)
#define __getcredentials(task) __IUserGroup->getcredentials(task) 

#endif /* _USERGROUP_HEADERS_H */
