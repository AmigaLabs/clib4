/*
 * $Id: usergroup_getpwnam_r.c,v 1.0 2023-04-13 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#define FIX(x) (pw->pw_##x = pw->pw_##x - line + buffer)
#define MAX_VAL_LEN 64
#define MAX_KEY_LEN 64

static unsigned int atou(char **s) {
    if (*s == NULL)
        return 0;

    unsigned int x;
    for (x = 0; **s - '0' < 10U; ++*s) x = 10 * x + (**s - '0');
    return x;
}

static int find_any_key_value(const char *str, char *key, char *value) {
    char junk[256] = {0};
    const char *search = str;
    while (*search != '\0') {
        int offset;
        if (sscanf(search, " %31[a-zA-Z_0-9]=%63s%n", key, value, &offset) == 2)
            return(search + offset - str);
        int rc;
        if ((rc = sscanf(search, "%255s%n", junk, &offset)) != 1)
            return EOF;
        search += offset;
    }

    return EOF;
}

static char *find_key_value(const char *str, const char *key, char *value) {
    char found[MAX_KEY_LEN + 1] = {0};
    int offset = -1;
    const char *search = str;
    while ((offset = find_any_key_value(search, found, value)) > 0) {
        if (strcmp(found, key) == 0)
            return (search + offset - str);
        search += offset;
    }
    if (offset == EOF)
        return NULL;
    return value;
}

static int __getpwent_a(FILE *f, struct passwd *pw, char **line, size_t *size, struct passwd **res) {
    char *s;
    int ret = 0;
    ssize_t l;

    for (;;) {
        if ((l=getline(line, size, f)) < 0) {
            ret = ferror(f) ? errno : 0;
            free(*line);
            *line = 0;
            pw = 0;
            break;
        }
        line[0][l-1] = 0;

        s = line[0];
        printf("S=%s\n", s);
        if (s[0] == '#')
            continue;

        char *value = malloc(MAX_VAL_LEN + 1);

        pw->pw_name = ((value = find_key_value(s, "NAME", value)) == NULL ? "anonymous" : value);
        memset(value, 0, MAX_VAL_LEN + 1);

        pw->pw_passwd = ((value = find_key_value(s, "PASSWORD", value)) == NULL ? "*" : value);
        memset(value, 0, MAX_VAL_LEN + 1);

        pw->pw_uid = ((value = find_key_value(s, "UID", value)) == NULL ? 0 : atou(&value));
        memset(value, 0, MAX_VAL_LEN + 1);

        pw->pw_gid = ((value = find_key_value(s, "GID", value)) == NULL ? 0 : atou(&value));
        memset(value, 0, MAX_VAL_LEN + 1);

        pw->pw_gecos = ((value = find_key_value(s, "GECOS", value)) == NULL ? pw->pw_name : value);
        memset(value, 0, MAX_VAL_LEN + 1);

        pw->pw_dir = ((value = find_key_value(s, "DIR", value)) == NULL ? "/sys" : value);
        memset(value, 0, MAX_VAL_LEN + 1);

        pw->pw_shell = ((value = find_key_value(s, "SHELL", value)) == NULL ? "CLI" : value);

        free(value);

        // Debug
        printf("Linea: %s\n", s);
        printf("NAME: %s\n", pw->pw_name);
        printf("PASSWORD: %s\n", pw->pw_passwd);
        printf("UID: %d\n", pw->pw_uid);
        printf("GID: %d\n", pw->pw_gid);
        printf("GECOS: %s\n", pw->pw_gecos);
        printf("DIR: %s\n", pw->pw_dir);
        printf("SHELL: %s\n", pw->pw_shell);
        printf("\n");
    }

    *res = pw;
    if (ret)
        __set_errno(ret);
    return ret;
}

static int __getpw_a(const char *name, struct passwd *pw, char **buf, size_t *size, struct passwd **res) {
    int ret = 0;
    FILE *file = fopen("Devs:Internet/users", "r");
    if (file == NULL) {
        ret = errno;
        goto out;
    }

    while (!(ret = __getpwent_a(file, pw, buf, size, res)) && *res) {
        if (name && !strcmp(name, (*res)->pw_name)) {
            printf("Found %s\n", name);
            break;
        }
    }

    fclose(file);

out:
    *res = pw;
    if (ret)
        __set_errno(ret);
    return ret;
}

int
getpwnam_r(const char *name, struct passwd *pw, char *buffer, size_t bufsize, struct passwd **result) {
    char *line = 0;
    size_t len = 0;

    int ret = __getpw_a(name, pw, &line, &len, result);
    if (*result && bufsize < len) {
        *result = 0;
        ret = ERANGE;
    }
    if (*result) {
        memcpy(buffer, line, len);
        FIX(name);
        FIX(passwd);
        FIX(gecos);
        FIX(dir);
        FIX(shell);
    }
    free(line);

    if (ret)
        __set_errno(ret);
    return ret;
}