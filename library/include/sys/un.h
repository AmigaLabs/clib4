#ifndef _UN_H
#define _UN_H

#include <features.h>

#define UNIX_PATH_MAX 108

/*
 * Definitions for UNIX IPC domain.
 */
struct sockaddr_un {
    sa_family_t sun_family;         /* AF_UNIX */
    char sun_path[UNIX_PATH_MAX];   /* path name (gag) */
};

/* actual length of an initialized sockaddr_un */
#define SUN_LEN(s) (2 + strlen((s)->sun_path))

#endif