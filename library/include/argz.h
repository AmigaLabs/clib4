/*
 * $Id: argz.h,v 1.0 2021-01-21 10:03:49 clib4devs Exp $
*/
#ifndef _ARGZ_H
#define _ARGZ_H

#include <features.h>
#include <errno.h>
#include <sys/types.h>

__BEGIN_DECLS

extern error_t argz_create(char *const argv[], char **argz, size_t *argz_len);
extern error_t argz_create_sep(const char *string, int sep, char **argz, size_t *argz_len);
extern size_t argz_count(const char *argz, size_t argz_len);
extern void argz_extract(char *argz, size_t argz_len, char **argv);
extern void argz_stringify(char *argz, size_t argz_len, int sep);
extern error_t argz_add(char **argz, size_t *argz_len, const char *str);
extern error_t argz_add_sep(char **argz, size_t *argz_len, const char *str, int sep);
extern error_t argz_append(char **argz, size_t *argz_len, const char *buf, size_t buf_len);
extern error_t argz_delete(char **argz, size_t *argz_len, char *entry);
extern error_t argz_insert(char **argz, size_t *argz_len, char *before, const char *entry);
extern char *argz_next(char *argz, size_t argz_len, const char *entry);
extern error_t argz_replace(char **argz, size_t *argz_len, const char *str, const char *with, unsigned *replace_count);

__END_DECLS

#endif // _ARGZ_H