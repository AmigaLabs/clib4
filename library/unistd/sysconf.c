/*
 * $Id: unistd_sysconf.c,v 1.1 2022-09-09 10:09:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/syslimits.h>
#include <netdb.h>
#include <pthread.h>

long 
sysconf(int name) {
    int retval = -1;
    ULONG query;

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
            GetCPUInfoTags(GCIT_ExecPageSize, (ULONG)&query, TAG_DONE);
            break;
        case _SC_TZNAME_MAX:
            return MAX_TZSIZE;
        case _SC_NPROCESSORS_CONF:
            GetCPUInfoTags(GCIT_NumberOfCPUs, (ULONG)&query, TAG_DONE);
            break;
        default:
            __set_errno(EINVAL);
            break;
    }
    retval = query;

    return retval;
}