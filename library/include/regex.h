#ifndef _REGEX_H
#define _REGEX_H

#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

typedef int regoff_t;

typedef struct re_pattern_buffer {
    size_t re_nsub;
    void *__opaque, *__padding[4];
    size_t __nsub2;
    char __padding2;
} regex_t;

typedef struct {
    regoff_t rm_so;
    regoff_t rm_eo;
} regmatch_t;

#define REG_EXTENDED    1
#define REG_ICASE       2
#define REG_NEWLINE     4
#define REG_NOSUB       8

#define REG_NOTBOL      1
#define REG_NOTEOL      2

#define REG_OK          0
#define REG_NOMATCH     1
#define REG_BADPAT      2
#define REG_ECOLLATE    3
#define REG_ECTYPE      4
#define REG_EESCAPE     5
#define REG_ESUBREG     6
#define REG_EBRACK      7
#define REG_EPAREN      8
#define REG_EBRACE      9
#define REG_BADBR       10
#define REG_ERANGE      11
#define REG_ESPACE      12
#define REG_BADRPT      13

#define REG_ENOSYS      -1

extern int regcomp(regex_t * preg, const char * regex, int cflags);
extern int regexec(const regex_t *, const char *, size_t, regmatch_t *, int);
extern void regfree(regex_t *preg);
extern size_t regerror(int e, const regex_t * preg, char * buf, size_t size);

__END_DECLS

#endif
