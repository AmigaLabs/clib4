/*
 * $Id: stdio_protos.h,v 1.19 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_PROTOS_H
#define _STDIO_PROTOS_H

#ifndef UTILITY_HOOKS_H
#include <utility/hooks.h>
#endif /* UTILITY_HOOKS_H */

#ifndef _STDIO_H
#include <stdio.h>
#endif /* _STDIO_H */

#ifndef _MATH_FP_SUPPORT_H
#include "math_fp_support.h"
#endif /* _MATH_FP_SUPPORT_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _STDIO_HEADERS_H

/* Forward declarations */
struct file_action_message;
struct iob;
struct fd;

#endif /* _STDIO_HEADERS_H */

void __close_all_files(struct _clib4 *__clib4);
extern int __translate_io_error_to_errno(LONG io_error);
extern int __translate_access_io_error_to_errno(LONG io_error);
extern struct fd *__get_file_descriptor(struct _clib4 *__clib4, int file_descriptor);
extern struct fd *__get_file_descriptor_dont_resolve(struct _clib4 *__clib4, int file_descriptor);
extern int64_t __iob_hook_entry(struct _clib4 *__clib4, struct iob *iob, struct file_action_message *fam);
extern int64_t __fd_hook_entry(struct _clib4 *__clib4, struct fd *fd, struct file_action_message *fam);
extern void __initialize_fd(struct fd *fd, file_action_fd_t action_function, BPTR default_file, ULONG flags, struct SignalSemaphore *lock);
extern BOOL __is_valid_fd(struct _clib4 *__clib4, struct fd *fd);
extern int __find_vacant_fd_entry(struct _clib4 *__clib4);
extern void __initialize_iob(struct iob *iob, file_action_iob_t action_function, STRPTR custom_buffer, STRPTR buffer, int64_t buffer_size, int file_descriptor, int slot_number, ULONG flags, struct SignalSemaphore *lock);
extern BOOL __is_valid_iob(struct _clib4 *__clib4, struct iob *iob);
extern int __find_vacant_iob_entry(struct _clib4 *__clib4);
extern int __grow_fd_table(struct _clib4 *__clib4, int max_fd);
extern int __grow_iob_table(struct _clib4 *__clib4, int max_fd);
extern int __open_iob(struct _clib4 *__clib4, const char *filename, const char *mode, int file_descriptor, int slot_number);
extern int __fill_iob_read_buffer(struct _clib4 *__clib4, struct iob *file);
extern int __drop_iob_read_buffer(struct _clib4 *__clib4, struct iob *file);
extern int __flush_iob_write_buffer(struct _clib4 *__clib4, struct iob *file);
extern int __fgetc_check(struct _clib4 *__clib4, FILE *stream);
extern int __fgetc(struct _clib4 *__clib4, FILE *stream);
extern int __fputc_check(struct _clib4 *__clib4, FILE *stream);
extern int __fputc(struct _clib4 *__clib4, int c, FILE *stream, int buffer_mode);
extern int64_t __sscanf_hook_entry(struct _clib4 *__clib4, struct iob *string, struct file_action_message *fam);
extern int64_t __vasprintf_hook_entry(struct _clib4 *__clib4, struct iob *string, struct file_action_message *fam);
extern int64_t __vsprintf_hook_entry(struct _clib4 *__clib4, struct iob *string, struct file_action_message *fam);
extern int64_t __vsnprintf_hook_entry(struct _clib4 *__clib4, struct iob *string, struct file_action_message *fam);
extern void __duplicate_fd(struct fd *duplicate_fd, struct fd *original_fd);
extern int64_t __grow_file_size(struct _clib4 *__clib4, struct fd *fd, int num_bytes);
extern int __sync_fd(struct fd *fd, int mode);
extern int __handle_record_locking(struct _clib4 *__clib4, int cmd, struct flock *l, struct fd *fd, int *error_ptr);
extern void __remove_fd_alias(struct _clib4 *__clib4, struct fd *fd);
extern int __stdio_file_init(void);
extern int __flush_all_files(struct _clib4 *__clib4, int buffer_mode);
extern int __flush_r(struct _clib4 *__clib4, FILE *stream);

#ifdef __USE_LARGEFILE64
/* stat_fchmod.c */
void __convert_stat64_to_stat(const struct stat64 *st64, struct stat *st);
#endif

#endif /* _STDIO_PROTOS_H */
