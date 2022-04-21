/*
 * $Id: debug_level.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
 */

/* By default, show and test only assertions. */
#ifndef DEBUG
int __debug_level = 0;
#else
int __debug_level = 2;
#endif
