#include <stdint.h>
#include <features.h>

HIDDEN uint64_t __rand48_step(unsigned short *xi, unsigned short *lc);
extern HIDDEN unsigned short __seed48[7];
