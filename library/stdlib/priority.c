/*
 * $Id: stdlib_priority.c,v 1.3 2006-01-08 12:04:26 clib2devs Exp $
*/

/* We use an invalid priority value to indicate "do not change the program's
   task priority". */
int __priority = 256;
