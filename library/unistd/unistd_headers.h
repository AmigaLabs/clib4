/*
 * $Id: unistd_headers.h,v 2.0 2023-05-03 09:40:06 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#define _UNISTD_HEADERS_H

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <poll.h>
#include <sys/param.h> // MAX

extern int __set_current_path(const char * path_name);
extern int __strip_double_slash(char * file_name,int len);
extern int __time_delay(ULONG timercmd, struct timeval *tv);
extern long __pathconf(struct MsgPort *port,int name);
extern int itimer_real_task(void);
extern void killitimer(void);
extern int __poll(struct pollfd *fds, nfds_t nfds, int timeout, uint32_t *signals);

extern void do_path_expansion(BPTR pathLock, char *fileName, int bufferSize, char *buffer);
extern BOOL dir_contains(BPTR dirLock, char *fileName);
extern int32 __search_command_hook_function(struct Hook *hook, APTR reserved, struct SearchCmdPathListMsg *message);
extern int __search_expand_command_path(char const **name_ptr, char *replacement_buffer, size_t replacement_buffer_size);

/* Forward declarations of helper functions for spawn* functions */
extern BOOL string_needs_quoting(const char *string, size_t len);
extern void build_arg_string(char *const argv[], char *arg_string);
extern size_t count_extra_escape_chars(const char *string, size_t len);
extern size_t get_arg_string_length(char *const argv[]);

#endif /* _UNISTD_HEADERS_H */
