#ifndef __CHILDREN_H__
#define __CHILDREN_H_

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "clib4.h"

BOOL insertSpawnedChildren(uint32 pid, uint32 ppid, uint32 gid);
struct Clib4Children *findSpawnedChildrenByPid(uint32 pid);
struct Clib4Children *findSpawnedChildrenByGid(uint32 pid, uint32 gid);
void spawnedProcessExit(int32 rc, int32 data UNUSED);
void spawnedProcessEnter(int32 entry_data UNUSED);

#endif