#ifndef _STDIO_EXT_H
#define _STDIO_EXT_H

#include <features.h>

__BEGIN_DECLS

#include <stdio.h>

#define FSETLOCKING_QUERY 0
#define FSETLOCKING_INTERNAL 1
#define FSETLOCKING_BYCALLER 2

extern void _flushlbf(void);
extern int __fsetlocking(FILE *, int);
extern int __fwriting(FILE *);
extern int __freading(FILE *);
extern int __freadable(FILE *);
extern int __fwritable(FILE *);
extern int __flbf(FILE *);
extern size_t __fbufsize(FILE *);
extern size_t __fpending(FILE *);
extern int __fpurge(FILE *);
extern int fpurge(FILE *);
extern void __fseterr(FILE *);

extern size_t __freadahead(FILE *);
extern void __freadptrinc(FILE *f, size_t inc);

__END_DECLS

#endif
