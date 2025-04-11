/*
 * $Id: mman_shm_open.c,v 1.0 2025-04-11 20:17:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/mman.h>
#include <fcntl.h>

static
void sanitize_shm_name(const char *input, char *output, size_t max_len) {
    const char *name_start = input;

    // Skip leading '/'
    while (*name_start == '/') {
        name_start++;
    }

    snprintf(output, max_len, "T:%s", name_start);
    output[max_len - 1] = '\0';  // Ensure null-termination
}

int
shm_open(const char *name, int oflag, mode_t mode) {
    char sanitized_name[PATH_MAX] = {0};
    sanitize_shm_name(name, sanitized_name, PATH_MAX);

    int fd = open(sanitized_name, oflag, mode);
    if (fd > 0) {
        struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
        if (res) {
            struct ShmOpenMap shmOpenItem;
            shmOpenItem.fd = fd;
            strncpy(shmOpenItem.name, sanitized_name, PATH_MAX);
            hashmap_set(res->shmFileMap, &shmOpenItem);
        }
    }
    return fd;
}