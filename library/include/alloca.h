#ifndef _ALLOCA_H
#define _ALLOCA_H       1

#define __need_size_t
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#if defined(alloca)
#undef alloca
#endif /* alloca */

#define alloca(size) __builtin_alloca(size)

__END_DECLS

#endif /* alloca.h */