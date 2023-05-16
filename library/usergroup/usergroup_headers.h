/*
 * $Id: usergroup_headers.h,v 1.8 2006-01-08 12:04:27 clib2devs Exp $
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

#ifndef _PWD_H
#include <pwd.h>
#endif /* _PWD_H */

#ifndef _GRP_H
#include <grp.h>
#endif /* _GRP_H */

#define DECLARE_USERGROUPBASE() \
	struct Library   UNUSED	*UserGroupBase  = __CLIB2->__UserGroupBase; \
	struct UserGroupIFace 	*IUserGroup	    = __CLIB2->__IUserGroup


#ifndef _PATH_SERVICES
#define _PATH_USERS  "Devs:Internet/users"
#endif

#include <interfaces/usergroup.h>

#define __ug_SetupContextTagList(name, tags) __CLIB2->__IUserGroup->ug_SetupContextTagList(name, tags)
#define __ug_GetErr() __CLIB2->__IUserGroup->ug_GetErr()
#define __ug_StrError(err) __CLIB2->__IUserGroup->ug_StrError(err)
#define __getuid() __CLIB2->__IUserGroup->getuid()
#define __geteuid() __CLIB2->__IUserGroup->geteuid()
#define __setreuid(real, effective) __CLIB2->__IUserGroup->setreuid(real, effective)
#define __setuid(uid) __CLIB2->__IUserGroup->setuid(uid)
#define __getgid() __CLIB2->__IUserGroup->getgid()
#define __getegid() __CLIB2->__IUserGroup->getegid()
#define __setregid(real, effective) __CLIB2->__IUserGroup->setregid(real, effective)
#define __setgid(gid) __CLIB2->__IUserGroup->setgid(gid)
#define __getgroups(gidsetlen, gidset) __CLIB2->__IUserGroup->getgroups(gidsetlen, gidset)
#define __setgroups(gidsetlen, gidset) __CLIB2->__IUserGroup->setgroups(gidsetlen, gidset)
#define __initgroups(name, basegid) __CLIB2->__IUserGroup->initgroups(name, basegid)
#define __getpwnam(login) __CLIB2->__IUserGroup->getpwnam(login)
#define __getpwuid(uid) __CLIB2->__IUserGroup->getpwuid(uid)
#define __setpwent() __CLIB2->__IUserGroup->setpwent()
#define __getpwent() __CLIB2->__IUserGroup->getpwent()
#define __endpwent() __CLIB2->__IUserGroup->endpwent()
#define __getgrnam(name) __CLIB2->__IUserGroup->getgrnam(name)
#define __getgrgid(gid) __CLIB2->__IUserGroup->getgrgid(gid)
#define __setgrent() __CLIB2->__IUserGroup->setgrent()
#define __getgrent() __CLIB2->__IUserGroup->getgrent()
#define __endgrent() __CLIB2->__IUserGroup->endgrent()
#define __crypt(key, set) __CLIB2->__IUserGroup->crypt(key, set)
#define __ug_GetSalt(user, buf, size) __CLIB2->__IUserGroup->ug_GetSalt(user, buf, size)
#define __getpass(prompt) __CLIB2->__IUserGroup->getpass(prompt)
#define __umask(mask) __CLIB2->__IUserGroup->umask(mask)
#define __getumask() __CLIB2->__IUserGroup->getumask()
#define __setsid() __CLIB2->__IUserGroup->setsid()
#define __getpgrp() __CLIB2->__IUserGroup->getpgrp()
#define __getlogin() __CLIB2->__IUserGroup->getlogin()
#define __setlogin(name) __CLIB2->__IUserGroup->setlogin(name)
#define __setutent() __CLIB2->__IUserGroup->setutent()
#define __getutent() __CLIB2->__IUserGroup->getutent()
#define __endutent() __CLIB2->__IUserGroup->endutent()
#define __getlastlog(uid) __CLIB2->__IUserGroup->getlastlog(uid)
#define __setlastlog(uid, name, host) __CLIB2->__IUserGroup->setlastlog(uid, name, host)
#define __getcredentials(task) __CLIB2->__IUserGroup->getcredentials(task)

#endif /* _USERGROUP_HEADERS_H */
