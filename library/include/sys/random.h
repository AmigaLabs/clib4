#ifndef _SYS_RANDOM_H
#define _SYS_RANDOM_H

#include <features.h>
#include <stddef.h>

__BEGIN_DECLS

#define GRND_NONBLOCK	0x0001
#define GRND_RANDOM	0x0002
#define GRND_INSECURE	0x0004

ssize_t getrandom(void *, size_t, unsigned);

__END_DECLS

#endif
