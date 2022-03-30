/* $VER: inline4/sysvipc.h 51.4 (25.02.107) By Peter Bengtsson
 */
#ifndef INLINE4_SYSVIPC_H
#define INLINE4_SYSVIPC_H

/*
** This file was manually edited by Peter Bengtsson
** who blatantly disregarded the warning not to edit.
**
*/ 

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef EXEC_EXEC_H
#include <exec/exec.h>
#endif
#ifndef EXEC_INTERFACES_H
#include <exec/interfaces.h>
#endif

#ifndef LIBRARIES_SYSVIPC_H
#include <libraries/sysvipc.h>
#endif

#undef shmat
#undef shmdt
#undef shmget
#undef shmctl
#undef shmids

#undef msgget
#undef msgsnd
#undef msgrcv
#undef msgctl
#undef msgsnap
#undef msgids

#undef semget
#undef semop
#undef semtimedop
#undef semctl
#undef semids

#undef ftok

/* Inline macros for Interface "main" */
#define shmat(shmid, prefaddr, flags) ISysVIPC->shmat(shmid, prefaddr, flags)
#define shmdt(shmaddr) ISysVIPC->shmdt(shmaddr)
#define shmget(key, size, flags) ISysVIPC->shmget(key, size, flags)
#define shmctl(shmid, cmd, cbuf) ISysVIPC->shmctl(shmid, cmd, cbuf)
#define shmids(buf, nids, idcnt) ISysVIPC->shmids(buf, nids, idcnt)
#define msgget(key, flags) ISysVIPC->msgget(key, flags)
#define msgsnd(key, msg, size, msgflag) ISysVIPC->msgsnd(key, msg, size, msgflag)
#define msgrcv(key, msg, size, type, flags) ISysVIPC->msgrcv(key, msg, size, type, flags)
#define msgctl(msgqid, cmd, cbuf) ISysVIPC->msgctl(msgqid, cmd, cbuf)
#define msgsnap(msgqid, buf, size, type) ISysVIPC->msgsnap(msgqid, buf, size, type)
#define msgids(buf, nids, idcnt) ISysVIPC->msgids(buf, nids, idcnt)
#define semget(key,nsems,flags)	ISysVIPC->semget(key,nsems,flags)
#define semop(semid,ops,nops)	ISysVIPC->semop(semid,ops,nops)
#define semtimedop(semid,ops,nops,to)	ISysVIPC->semtimedop(semid,ops,nops,to)
#define semctl(semid,semnum,cmd,aun)	ISysVIPC->semctl(semid,semnum,cmd,aun)
#define semids(buf,nids,idcnt)	ISysVIPC->semids(buf,nids,idcnt)
#define GetIPCErr	ISysVIPC->GetIPCErr
#define ftok(path,c) ISysVIPC->ftok(path,c)
#define IPCControl(taglist) ISysVIPC->IPCControl(taglist)
#if ( ( __STDC_VERSION__ +0 ) >= 199901L ) || (__GNUC__ >= 3)
#define IPCControlTags(tag0,...) ISysVIPC->IPCControlTags(tag0,__VA_ARGS__)
#else
#define IPCControlTags	ISysVIPC->IPCControlTags
#endif

#endif /* INLINE4_SYSVIPC_H */
