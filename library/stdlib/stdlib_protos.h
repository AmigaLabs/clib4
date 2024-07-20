/*
 * $Id: stdlib_protos.h,v 1.18 2010-10-20 13:50:17 clib4devs Exp $
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

#include <dos.h>

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

extern void reent_init(struct _clib4 *__clib4);
extern void reent_exit(struct _clib4 *__clib4, BOOL fallback);

/* signal_checkabort.c */
extern int32 _start(STRPTR argstring, int32 arglen, struct ExecBase *sysbase);

extern void __abort(void);

/* stdlib_assertion_failure.c */
extern void __assertion_failure(const char *file_name, int line_number, const char *expression);

/* stdlib_set_process_window.c */
extern APTR __set_process_window(APTR new_window_pointer);

/* stdlib_set_errno.c */
extern void __set_errno(int new_errno);
extern void __set_errno_r(struct _clib4 *__clibt4, int new_errno);

/* stdlib_get_errno.c */
extern int __get_errno(void);

/* stdlib_semaphore.c */
extern struct SignalSemaphore *__create_semaphore(void);
extern void __delete_semaphore(struct SignalSemaphore *semaphore);

/* random functions */
extern void *savestate(void);
extern void loadstate(uint32_t *state);

extern void __srandom(unsigned seed);
extern void *__malloc_r(struct _clib4 *__clib4, size_t size);

extern uint32_t lcg31(uint32_t x);
extern uint64_t lcg64(uint64_t x);

extern char *__randname(char *template);

extern int *__h_errno_r(struct _clib4 *__clib4);
extern int *__errno_r(struct _clib4 *__clib4);

#define errno_r (*__errno_r(__clib4))
extern int *__errno_r(struct _clib4 *__clib4);
#define h_errno_r (*__h_errno_r(__clib4))
extern int *__h_errno_r(struct _clib4 *__clib4);

extern struct DOSIFace *IDOS;

inline void
__check_abort(void) {
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->__check_abort_enabled && CheckSignal(__clib4->__break_signal_mask)) {
        raise(SIGINT);
    }
}

/* Faster __check_abort version used when __clib4 is available in the caller function */
inline void
__check_abort_f(struct _clib4 *__clib4) {
    if (__clib4->__check_abort_enabled && CheckSignal(__clib4->__break_signal_mask)) {
        raise(SIGINT);
    }
}

#endif /* _STDLIB_PROTOS_H */
