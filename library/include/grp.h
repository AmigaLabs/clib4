/*
 * $Id: grp.h,v 1.7 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _GRP_H
#define _GRP_H

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

struct group
{
	char *	gr_name;	/* group name */
	char *	gr_passwd;	/* group password */
	gid_t	gr_gid;		/* group id */
	char **	gr_mem;		/* group members */
};

/****************************************************************************/

/*
 * The following prototypes may clash with the bsdsocket.library or
 * usergroup.library API definitions.
 */

extern void endgrent(void);
extern struct group *getgrent(void);
extern struct group *getgrgid(gid_t gid);
extern struct group *getgrnam(const char *name);
extern void setgrent(void);

#ifdef __GNUC__
 #ifdef __PPC__
  #pragma pack()
 #endif
#elif defined(__VBCC__)
 #pragma default-align
#endif

__END_DECLS

#endif /* _GRP_H */
