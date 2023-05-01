/*
  $Id: pthread_num_processor_np.c,v 1.00 2023-05-01 12:09:49 clib2devs Exp $
*/


#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <pthread.h>

/*
* Get the number of CPUs available to the process.
*/

int
pthread_num_processors_np(void) {
    ULONG count;

    GetCPUInfoTags(GCIT_NumberOfCPUs, &count, TAG_DONE);

    return (int) count;
}