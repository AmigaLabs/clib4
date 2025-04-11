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
shm_unlink(const char *name) {
    char sanitized_name[PATH_MAX] = {0};
    sanitize_shm_name(name, sanitized_name, PATH_MAX);

    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
    if (res) {
        struct ShmOpenMap shmItem;
        strncpy(shmItem.name, sanitized_name, PATH_MAX);
        struct ShmOpenMap *item = (struct ShmOpenMap *) hashmap_get(res->shmFileMap, &shmItem);
        if (item != NULL) {
            close(item->fd);
            return 0;
        }
    }
    __set_errno(ENOENT);
    return -1;
}