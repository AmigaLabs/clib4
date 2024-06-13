#ifndef __CHILDREN_H__
#define __CHILDREN_H_

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

BOOL insertSpawnedChildren(uint32 pid, uint32 gid);

#endif