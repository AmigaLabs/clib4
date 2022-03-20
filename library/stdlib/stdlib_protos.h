/*
 * $Id: stdlib_protos.h,v 1.18 2010-10-20 13:50:17 clib2devs Exp $
*/

#ifndef _STDLIB_PROTOS_H
#define _STDLIB_PROTOS_H

/****************************************************************************/

#ifndef EXEC_TASKS_H
#include <exec/tasks.h>
#endif /* EXEC_TASKS_H */

/****************************************************************************/

#ifndef _STDDEF_H
#include <stddef.h>
#endif /* _STDDEF_H */

/****************************************************************************/

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

/* stdio_init_exit.c */
extern int __stdio_init(void);

/****************************************************************************/

/* stdlib_init_exit.c */
extern int __stdlib_init(void);
extern void __stdlib_exit(void);

/****************************************************************************/

extern int __timezone_init(void);
extern void __timezone_exit(void);

/****************************************************************************/

/* stdlib_atexit.c */
extern void __exit_trap_trigger(void);

/****************************************************************************/

/* socket_init_exit.c */
extern int __socket_init(void);

/****************************************************************************/

/* stdlib_showerror.c */
extern void __show_error(const char *message);

/****************************************************************************/

/* stdlib_swap_stack.c/stdlib_swap_stack.s/stdlib_swap_stack.asm */
extern int __swap_stack_and_call(struct StackSwapStruct *stk, APTR function);

/****************************************************************************/

/* stdlib_get_sp.c/stdlib_get_sp.s/stdlib_get_sp.asm */
extern void *__get_sp(void);

/****************************************************************************/

/* stdlib_wildcard_expand.c */
extern int __wildcard_quote_parameter(unsigned int parameter);
extern int __wildcard_expand_init(void);

/****************************************************************************/

/* stdlib_stackextension.c */
extern int __stk_init(void);

/****************************************************************************/

/* stdlib_stack_usage.c */
extern void __stack_usage_init(struct StackSwapStruct *stk);
extern void __stack_usage_exit(void);

/****************************************************************************/

/* stdlib_exit.c */
extern void _exit(int return_code);

void reent_init(void);
void reent_exit(void);

/* stdlib_constructor_begin.c */
void _init(void);
void _fini(void);
void shared_obj_init(void);
void shared_obj_exit(void);

/****************************************************************************/

/* stdlib_arg.c */
extern int __arg_init(void);

/****************************************************************************/

/* stdlib_malloc.c */
extern size_t __get_allocation_size(size_t size);
extern void *__allocate_memory(size_t size, BOOL never_free);

/* stdlib_free.c */
extern struct MemoryNode *__find_memory_node(void *address);
extern void __check_memory_allocations();
extern void __free_memory(void *ptr, BOOL force);
extern void __free_memory_node(struct MemoryNode *mn);

/****************************************************************************/

/* signal_checkabort.c */
extern void __check_abort(void);

/* stdlib_main.c */
extern int _main(void);
extern int _start(char *args, int arglen, struct ExecBase *sysBase);
extern void _fini(void);

/****************************************************************************/

/* stdlib_assertion_failure.c */
extern void __assertion_failure(const char *file_name, int line_number, const char *expression);

/****************************************************************************/

/* stdlib_termination_message.c */
extern void __print_termination_message(const char *termination_message);

/****************************************************************************/

/* stdlib_set_process_window.c */
extern APTR __set_process_window(APTR new_window_pointer);

/****************************************************************************/

/* stdlib_set_errno.c */
extern void __set_errno(int new_errno);

/****************************************************************************/

/* stdlib_get_errno.c */
extern int __get_errno(void);

/****************************************************************************/

/* stdlib_semaphore.c */
extern struct SignalSemaphore *__create_semaphore(void);
extern void __delete_semaphore(struct SignalSemaphore *semaphore);

/****************************************************************************/

/* random functions */
extern void *savestate(void);
extern void loadstate(uint32_t *state);

extern void __srandom(unsigned seed);

extern uint32_t lcg31(uint32_t x);
extern uint64_t lcg64(uint64_t x);

#endif /* _STDLIB_PROTOS_H */
