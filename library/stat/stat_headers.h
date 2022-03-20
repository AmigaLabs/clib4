/*
 * $Id: stat_headers.h,v 1.7 2006-11-13 09:25:28 clib2devs Exp $
*/

#ifndef _STAT_HEADERS_H
#define _STAT_HEADERS_H

/****************************************************************************/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

extern mode_t NOCOMMON __current_umask;

/****************************************************************************/

extern void __convert_file_info_to_stat(const struct MsgPort *file_system, const struct ExamineData *fib, struct stat *st);
extern BPTR __lock(const char *name, const int mode, int *link_length, char *real_name, size_t real_name_size);

/****************************************************************************/

#endif /* _STAT_HEADERS_H */
