#ifndef _ENVZ_H_
#define _ENVZ_H_

#include <features.h>
#include <errno.h>
#include <sys/types.h>

__BEGIN_DECLS

extern char *envz_entry(const char *envz, size_t envz_len, const char *name);
extern char *envz_get(const char *envz, size_t envz_len, const char *name);
extern error_t envz_add(char **envz, size_t *envz_len, const char *name, const char *value);
extern error_t envz_merge(char **envz, size_t *envz_len, const char *envz2, size_t envz2_len, int override);
extern void envz_remove(char **envz, size_t *envz_len, const char *name);
extern void envz_strip(char **envz, size_t *envz_len);

__END_DECLS

#endif
