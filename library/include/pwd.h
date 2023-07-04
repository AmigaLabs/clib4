/*
 * $Id: pwd.h,v 1.7 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _PWD_H
#define _PWD_H

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif /* _SYS_TYPES_H */

#include <features.h>

__BEGIN_DECLS

#ifdef __GNUC__
 #ifdef __PPC__
  #pragma pack(2)
 #endif
#elif defined(__VBCC__)
 #pragma amiga-align
#endif

/****************************************************************************/

struct passwd
{
	char *	pw_name;	/* Username */
	char *	pw_passwd;	/* Encrypted password */
	uid_t	pw_uid;		/* User ID */
	gid_t	pw_gid;		/* Group ID */
	char *	pw_gecos;	/* Real name etc */
	char *	pw_dir;		/* Home directory */
	char *	pw_shell;	/* Shell */
};

extern void endpwent(void);
extern struct passwd *getpwent(void);
extern struct passwd *getpwnam(const char *name);
extern int getpwnam_r(const char *name, struct passwd *pw, char *buffer, size_t bufsize, struct passwd **result);
extern struct passwd *getpwuid(uid_t uid);
extern int getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result);
extern void setpwent(void);

#ifdef __GNUC__
 #ifdef __PPC__
  #pragma pack()
 #endif
#elif defined(__VBCC__)
 #pragma default-align
#endif

__END_DECLS

#endif /* _PWD_H */
