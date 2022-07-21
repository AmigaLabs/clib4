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