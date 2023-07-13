/*
 * $Id: stdlib_protos.h,v 1.18 2010-10-20 13:50:17 clib2devs Exp $
*/

#ifndef _STDLIB_PROTOS_H
#define _STDLIB_PROTOS_H

#ifndef EXEC_TASKS_H
#include <exec/tasks.h>
#endif /* EXEC_TASKS_H */

#ifndef _STDDEF_H
#include <stddef.h>
#endif /* _STDDEF_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef PROTO_DOS_H
#include <proto/dos.h>
#endif /* PROTO_DOS_H */

extern int __timezone_init(void);
extern void __timezone_exit(void);

/* stdlib_atexit.c */
extern void __exit_trap_trigger(void);

/* stdlib_showerror.c */
extern void __show_error(const char *message);

/* stdlib_get_sp.c/stdlib_get_sp.s/stdlib_get_sp.asm */
extern void *__get_sp(void);

/* stdlib_wildcard_expand.c */
extern int __wildcard_quote_parameter(unsigned int parameter);
extern int __wildcard_expand_init(void);

/* stdlib_exit.c */
extern void _exit(int return_code);

extern void reent_init(struct _clib2 *__clib2);
extern void reent_exit(struct _clib2 *__clib2, BOOL fallback);

/* signal_checkabort.c */
extern void __check_abort(void);
extern int32 _start(STRPTR argstring, int32 arglen, struct ExecBase *sysbase);

/* stdlib_assertion_failure.c */
extern void __assertion_failure(const char *file_name, int line_number, const char *expression);

/* stdlib_set_process_window.c */
extern APTR __set_process_window(APTR new_window_pointer);

/* stdlib_set_errno.c */
extern void __set_errno(int new_errno);

/* stdlib_get_errno.c */
extern int __get_errno(void);

/* stdlib_semaphore.c */
extern struct SignalSemaphore *__create_semaphore(void);
extern void __delete_semaphore(struct SignalSemaphore *semaphore);

/* random functions */
extern void *savestate(void);
extern void loadstate(uint32_t *state);

extern void __srandom(unsigned seed);

extern uint32_t lcg31(uint32_t x);
extern uint64_t lcg64(uint64_t x);

extern char *__randname(char *template);

#endif /* _STDLIB_PROTOS_H */
