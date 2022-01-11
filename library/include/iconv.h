#ifndef _ICONV_H_
#define _ICONV_H_

#include <features.h>
#include <sys/types.h>

/* iconv_t: charset conversion descriptor type */
typedef _iconv_t iconv_t;

__BEGIN_DECLS

extern iconv_t iconv_open(const char *to, const char *from);
/* POSIX-1.2008:
 *  on posix-1 2008 iconv needs a char ** not a const char **
 */
extern size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
extern int iconv_close(iconv_t cd);

__END_DECLS

#endif