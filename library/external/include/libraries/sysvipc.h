/* $VER: libraries/sysvipc.h 53.7 (26.01.109) By Peter Bengtsson
 */

#ifndef	_SYSVIPC_H
#define	_SYSVIPC_H

#include <exec/types.h>
#include <utility/tagitem.h>

#include <time.h>

/* Error codes returned. */

#define	SV_EOK		(0)
#define	SV_EPERM		(1)
#define	SV_ENOENT	(2)
#define	SV_EINTR		(4)
#define	SV_E2BIG		(7)
#define	SV_ENOMEM	(12)
#define	SV_EACCESS	(13)
#define	SV_EFAULT	(14)
#define	SV_EEXIST	(17)
#define	SV_EINVAL	(22)
#define	SV_EFBIG		(27)
#define	SV_ENOSPC	(28)
#define	SV_EAGAIN	(35)
#define	SV_EMSGSIZE	(40)
#define	SV_EIDRM		(82)
#define	SV_ENOMSG	(83)

/* Common IPC defines. */

#define	IPC_CREAT	001000
#define	IPC_EXCL		002000
#define	IPC_NOWAIT	004000
#define	IPC_NOERROR	010000

/* Modeflags for extra things. */
#define	SHM_CLEAR			001000	/* shm segment has never been attached */
#define	IPC_DELETE			002000	/* resource is scheduled to be deleted */
#define	MSG_SENDWAIT		004000	/* message queue is full (waiting on send) */
#define	MSG_RECVWAIT		010000	/* message queue is empty (waiting on recv) */

#define	IPC_RMID		1
#define	IPC_SET		2
#define	IPC_STAT		3

#define	IPC_PRIVATE	((key_t)0x80000000L)

/* Shared memory */

#define	SHM_R		0400
#define	SHM_W		0200

#define	SHM_LOCK		11
#define	SHM_UNLOCK	12

/* Message Queues */

#define	MSG_R		0400
#define	MSG_W		0200

/* Semaphores */

#define	SEM_R		0400
#define	SEM_W		0200

/* for semctl() */

#define	GETVAL	4
#define	SETVAL	5
#define	GETPID	6
#define	GETNCNT	7
#define	GETZCNT	8
#define	SETALL	9
#define	GETALL	10

#define	SEM_UNDO	020000

/* IPC services control. */

#define	SVCTRL_BASETAG	(TAG_USER)
#define	SVCTRL_SMAXSHMSIZE	(SVCTRL_BASETAG+1)
#define	SVCTRL_SMAXQUEUESIZE (SVCTRL_BASETAG+2)
#define	SVCTRL_GMAXSHMSIZE	(SVCTRL_BASETAG+3)
#define	SVCTRL_GMAXQUEUESIZE (SVCTRL_BASETAG+4)
#define	SVCTRL_GUSEDSHMSIZE	(SVCTRL_BASETAG+5)

#endif
