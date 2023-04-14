/*
 * $Id: usergroup_getpwuid_r.c,v 1.0 2023-04-13 12:04:27 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

int
getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result) {
    struct name_translation_info buffer_nti;
    char *home;

    *result = NULL;

    /* we only emulate a single user, with uid 0 */
    if (uid != 0) {
        __set_errno(ENOENT);
        return ENOENT;
    }

    memset(pwd, 0, sizeof(*pwd));
    memset(buffer, 0, bufsize);

    pwd->pw_name = buffer;
    if (getlogin_r(buffer, bufsize) != 0) {
        __set_errno(ERANGE);
        return ERANGE;
    }
    size_t len = strlen(buffer) + 1;
    buffer = &buffer[len];
    bufsize -= len;

    pwd->pw_passwd = "*";
    pwd->pw_gid = pwd->pw_uid = uid;

    if (bufsize <= 0) {
        __set_errno(ERANGE);
        return ERANGE;
    }

    if ((home = getenv("HOME"))) {
        if (__translate_amiga_to_unix_path_name((const char **) &home, &buffer_nti) != 0) {
            __set_errno(ERANGE);
            return ERANGE;
        }
        strncpy(buffer, buffer_nti.substitute, bufsize);
    } else {
        strcpy(buffer, "/sys");
    }
    pwd->pw_dir = buffer;
    pwd->pw_shell = "CLI";

    *result = pwd;
    return 0;
}