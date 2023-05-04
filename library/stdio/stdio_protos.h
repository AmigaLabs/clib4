/*
 * $Id: stdio_protos.h,v 1.19 2006-01-08 12:04:24 clib2devs Exp $
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

/****************************************************************************/

/* stdio_init_exit.c */
void __close_all_files(void);

/****************************************************************************/

/* stdio_translateioerror.c */
extern int __translate_io_error_to_errno(LONG io_error);
extern int __translate_access_io_error_to_errno(LONG io_error);

/****************************************************************************/

/* stdio_get_file_descriptor.c */
extern struct fd *__get_file_descriptor(int file_descriptor);
extern struct fd *__get_file_descriptor_dont_resolve(int file_descriptor);

/****************************************************************************/

/* stdio_iobhookentry.c */
extern int64_t __iob_hook_entry(struct iob *iob, struct file_action_message *fam);

/****************************************************************************/

/* stdio_fdhookentry.c */
extern int64_t __fd_hook_entry(struct fd *fd, struct file_action_message *fam);

/****************************************************************************/

/* stdio_initializefd.c */
extern void __initialize_fd(struct fd *fd, file_action_fd_t action_function, BPTR default_file, ULONG flags, struct SignalSemaphore *lock);

/****************************************************************************/

/* stdio_findvacantfdentry.c */
extern BOOL __is_valid_fd(struct fd *fd);
extern int __find_vacant_fd_entry(void);

/****************************************************************************/

/* stdio_initializeiob.c */
extern void __initialize_iob(struct iob *iob, file_action_iob_t action_function, STRPTR custom_buffer, STRPTR buffer, int64_t buffer_size, int file_descriptor, int slot_number, ULONG flags, struct SignalSemaphore *lock);

/****************************************************************************/

/* stdio_findvacantiobentry.c */
extern BOOL __is_valid_iob(struct iob *iob);
extern int __find_vacant_iob_entry(void);

/****************************************************************************/

/* stdio_growfdtable.c */
extern int __grow_fd_table(int max_fd);

/****************************************************************************/

/* stdio_growiobtable.c */
extern int __grow_iob_table(int max_fd);

/****************************************************************************/

/* stdio_openiob.c */
extern int __open_iob(const char *filename, const char *mode, int file_descriptor, int slot_number);

/****************************************************************************/

/* stdio_filliobreadbuffer.c */
extern int __fill_iob_read_buffer(struct iob *file);

/****************************************************************************/

/* stdio_dropiobreadbuffer.c */
extern int __drop_iob_read_buffer(struct iob *file);

/****************************************************************************/

/* stdio_flushiobwritebuffer.c */
extern int __flush_iob_write_buffer(struct iob *file);

/****************************************************************************/

/* stdio_fgetc.c */
extern int __fgetc_check(FILE *stream);
extern int __fgetc(FILE *stream);

/****************************************************************************/

/* stdio_fputc.c */
extern int __fputc_check(FILE *stream);
extern int __fputc(int c, FILE *stream, int buffer_mode);

/****************************************************************************/

/* stdio_sscanf_hook_entry.c */
extern int64_t __sscanf_hook_entry(struct iob *string, struct file_action_message *fam);

/****************************************************************************/

/* stdio_vasprintf_hook_entry.c */
extern int64_t __vasprintf_hook_entry(struct iob *string, struct file_action_message *fam);

/****************************************************************************/

/* stdio_vsprintf_hook_entry.c */
extern int64_t __vsprintf_hook_entry(struct iob *string, struct file_action_message *fam);

/****************************************************************************/

/* stdio_vsnprintf_hook_entry.c */
extern int64_t __vsnprintf_hook_entry(struct iob *string, struct file_action_message *fam);

/****************************************************************************/

/* stdio_duplicate_fd.c */
extern void __duplicate_fd(struct fd *duplicate_fd, struct fd *original_fd);

/****************************************************************************/

/* stdio_parent_of_fh.c */
extern BPTR __safe_parent_of_file_handle(BPTR file_handle);

/****************************************************************************/

/* stdio_grow_file.c */
extern int64_t __grow_file_size(struct fd *fd, int num_bytes);

/****************************************************************************/

/* unistd_sync_fd.c */
extern int __sync_fd(struct fd *fd, int mode);

/****************************************************************************/

/* stdio_record_locking.c */
extern int __handle_record_locking(int cmd, struct flock *l, struct fd *fd, int *error_ptr);

/****************************************************************************/

/* stdio_remove_fd_alias.c */
extern void __remove_fd_alias(struct fd *fd);

/****************************************************************************/

/* stdio_file_init.c */
extern int __stdio_file_init(void);

/****************************************************************************/

/* stdio_flush_all_files.c */
extern int __flush_all_files(int buffer_mode);

/****************************************************************************/
#ifdef __USE_LARGEFILE64
/* stat_fchmod.c */
void __convert_stat64_to_stat(const struct stat64 *st64, struct stat *st);
#endif

#endif /* _STDIO_PROTOS_H */
