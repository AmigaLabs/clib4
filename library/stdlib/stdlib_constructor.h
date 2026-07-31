/*
 * $Id: stdlib_constructor.h,v 1.4 2023-02-11 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_CONSTRUCTOR_H
#define _STDLIB_CONSTRUCTOR_H

#define CONSTRUCTOR_SUCCEED() \
	return

// This can cause problems on libraries because of exit. We should find a better way to do this
#define CONSTRUCTOR_FAIL() \
	exit(RETURN_FAIL)
    
/****************************************************************************/

/* Initialization functions, called in this exact order from clib4_init()
   (shared_library/clib4.c, libOpen) */

void stdlib_memory_init(void);		/* STDLIB	- stdlib/malloc.c            */
void stdio_init(void);				/* STDIO	- stdio/init_exit.c          */
void stdio_file_init(void);		/* FILE		- stdio/file_init.c          */
void math_init(void);				/* MATH		- math/init_exit.c           */
void socket_init(void);			/* SOCKET	- socket/init_exit.c         */
void locale_init(void);			/* CLIB		- locale/init_exit.c         */
void usergroup_init(void);			/* CLIB		- usergroup/init_exit.c      */
void timezone_init(void);			/* CLIB		- time/timezone_init_exit.c  */
void unistd_init(void);			/* CLIB		- unistd/init_exit.c         */
void timer_init(void);				/* CLIB		- unistd/timer.c             */
void clock_init(void);				/* CLIB		- time/clock.c               */
void dirent_init(void);			/* CLIB		- dirent/closedir.c          */

/* Cleanup functions, called in this exact order from clib4_exit()
   (shared_library/clib4.c, libClose): the reverse of the initialization
   order above, plus the cleanup-only functions that have no matching
   initialization function */

void dirent_exit(void);			/* CLIB		- dirent/closedir.c          */
void timer_exit(void);				/* CLIB		- unistd/timer.c             */
void unistd_exit(void);			/* CLIB		- unistd/init_exit.c         */
void __wildcard_expand_exit(void);	/* CLIB		- unistd/wildcard_expand.c   */
void __chdir_exit(void);			/* CLIB		- unistd/chdir_exit.c        */
void __setenv_exit(void);			/* CLIB		- stdlib/setenv.c            */
void dcngettext_exit(void);		/* CLIB		- locale/dcngettext.c        */
void timezone_exit(void);			/* CLIB		- time/timezone_init_exit.c  */
void usergroup_exit(void);			/* CLIB		- usergroup/init_exit.c      */
void locale_exit(void);			/* CLIB		- locale/init_exit.c         */
void socket_exit(void);			/* SOCKET	- socket/init_exit.c         */
void stdio_exit(void);				/* STDIO	- stdio/init_exit.c          */
void stdlib_memory_exit(void);		/* STDLIB	- stdlib/malloc.c            */

/****************************************************************************/

#endif /* _STDLIB_CONSTRUCTOR_H */
