/*
 * $Id: unistd_sysconf.c,v 1.1 2024-08-14 10:09:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/syslimits.h>
#include <netdb.h>
#include <pthread.h>

long 
sysconf(int name) {
    ULONG query = -1;

    switch (name) {
        case _SC_ARG_MAX:
            return ARG_MAX;
        case _SC_HOST_NAME_MAX:
            return MAXHOSTNAMELEN;
        case _SC_CLK_TCK:
            return CLK_TCK;
        case _SC_OPEN_MAX:
            return FOPEN_MAX;
        case _SC_THREAD_KEYS_MAX:
            return PTHREAD_KEYS_MAX;
        case _SC_PAGESIZE:
            GetCPUInfoTags(GCIT_ExecPageSize, (ULONG) &query, TAG_DONE);
            break;
        case _SC_VERSION:
            return _POSIX_VERSION;
        case _SC_TZNAME_MAX:
            return MAX_TZSIZE;
        case _SC_NPROCESSORS_CONF:
        case _SC_NPROCESSORS_ONLN:
            GetCPUInfoTags(GCIT_NumberOfCPUs, (ULONG) &query, TAG_DONE);
            break;
        case _SC_TTY_NAME_MAX:
            return _POSIX_TTY_NAME_MAX;
        case _SC_GETPW_R_SIZE_MAX:
            break;
        default:
            __set_errno(EINVAL);
            break;
    }
    return query;
}