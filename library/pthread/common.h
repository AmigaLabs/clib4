#ifndef _COMMON_H
#define _COMMON_H

#undef NEWLIST
#define NEWLIST(_l)                                     \
do                                                      \
{                                                       \
    struct List *__aros_list_tmp = (struct List *)(_l), \
                *l = __aros_list_tmp;                   \
                                                        \
    l->lh_TailPred = (struct Node *)l;                \
    l->lh_Tail     = 0;                                 \
    l->lh_Head     = (struct Node *)&l->lh_Tail;      \
} while (0)

#define TIMESPEC_TO_TIMEVAL(tv, ts) {    \
    (tv)->Seconds = (ts)->tv_sec;        \
    (tv)->Microseconds = (ts)->tv_nsec / 1000; }

#endif

enum threadState
{
    THREAD_STATE_IDLE 		= 0,
    THREAD_STATE_RUNNING 	= 1,
    THREAD_STATE_JOINING	= 2,
    THREAD_STATE_TERMINATED	= 3,
    THREAD_STATE_CANCELED	= 4,
    THREAD_STATE_WAITING	= 5,
    THREAD_STATE_DESTRUCT   = 6,
};