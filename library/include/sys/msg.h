#ifndef _SYS_MSG_H
#define _SYS_MSG_H

#include <sys/cdefs.h>
#include <sys/shm.h>

__BEGIN_DECLS

/* ipcs ctl commands */
#define MSG_STAT 11
#define MSG_INFO 12

/* msgrcv options */
#define MSG_NOERROR 010000 /* no error if message is too big */
#define MSG_EXCEPT  020000  /* recv any msg except of specified type.*/
#define MSG_COPY    040000    /* copy (not remove) all queue messages */

extern int _msgctl(int msqid, int cmd, struct msqid_ds *buf);
extern int _msgget(key_t key, int msgflg);
extern ssize_t _msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
extern int _msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
extern int _msgsnap(int msqid, void *buf, size_t bufsz, long msgtyp);
extern int _msgids(int *buf, size_t nids, size_t  *pnids);

#define msgctl(a, b, c)         _msgctl(a, b, c)
#define msgget(a, b)            _msgget(a, b)
#define msgrcv(a, b, c, d, e)   _msgrcv(a, b, c, d, e)
#define msgsnd(a, b, c, d)      _msgsnd(a, b, c, d)
#define msgsnap(a, b, c, d)     _msgsnap(a, b, c, d)
#define msgids(a, b, c)         _msgids(a, b, c)

__END_DECLS

#endif // _SYS_MSG_H