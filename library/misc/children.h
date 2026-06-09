#ifndef __CHILDREN_H__
#define __CHILDREN_H_

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "clib4.h"

BOOL insertSpawnedChildren(uint32 pid, uint32 gid, const char *parentUuid, const char *fdInherit);
struct Clib4Children *findSpawnedChildrenByPid(uint32 pid);
struct Clib4Children *findSpawnedChildrenByGid(uint32 pid, uint32 gid);
void addSpawnedChildrenPipeHandle(uint32 pid, FILE *pipe);
pid_t findSpawnedChildrenPidByPipe(FILE *pipe);
void spawnedProcessExit(int32 rc, int32 data UNUSED);
void spawnedProcessEnter(int32 entry_data);
void import_inherited_fds_from_spec(struct _clib4 *__clib4, const char *spec);
void import_pending_fds_for_process(struct _clib4 *__clib4, uint32 pid, uint32 ppid);

struct spawnData {
	gid_t groupId;
	struct Task *parentTask;
	char parentUuid[UUID4_LEN + 1];
	char *fdInherit;
};

#endif