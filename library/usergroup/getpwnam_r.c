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

static const char* get_value(const char* line, const char* key, char* value, size_t value_size) {
    char pattern[128];
    sprintf(pattern, "%s=", key);
    char* match = strstr(line, pattern);
    if (match != NULL) {
        match += strlen(key) + 1;
        // Ignore space before value
        while (*match == ' ') {
            match++;
        }
        // Copy value til first space or NULL
        int i = 0;
        while (*match != '\0' && *match != ' ' && i < value_size - 1) {
            value[i++] = *match++;
        }
        value[i] = '\0';
    } else {
        // If key is not present return NULL
        value[0] = '\0';
    }
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
        if (s[0] == '#')
            continue;

        char name[MAX_VAL_LEN + 1] = "";
        char password[MAX_VAL_LEN + 1] = "";
        char uid[MAX_VAL_LEN + 1] = "";
        char gid[MAX_VAL_LEN + 1] = "";
        char gecos[MAX_VAL_LEN + 1] = "";
        char dir[MAX_VAL_LEN + 1] = "";
        char shell[MAX_VAL_LEN + 1] = "";

        get_value(s, "NAME", name, sizeof(name));
        pw->pw_name = (char*) valloc(MAX_VAL_LEN + 1);
        if (name[0] == '\0') {
            strcpy(pw->pw_name, "anonymous");
            pw->pw_name[9] = '\0';
        }
        else {
            strncpy(pw->pw_name, name, MAX_VAL_LEN);
            pw->pw_name[strlen(name)] = '\0';
        }

        get_value(s, "PASSWORD", password, sizeof(password));
        pw->pw_passwd = (char*) valloc(MAX_VAL_LEN + 1);
        if (password[0] == '\0') {
            strcpy(pw->pw_passwd, "*");
            pw->pw_passwd[1] = '\0';
        }
        else {
            strncpy(pw->pw_passwd, password, MAX_VAL_LEN);
            pw->pw_passwd[strlen(password)] = '\0';
        }

        get_value(s, "UID", uid, sizeof(uid));
        pw->pw_uid = uid[0] == '\0' ? 0 : atoi(uid);

        get_value(s, "GID", gid, sizeof(gid));
        pw->pw_gid = gid[0] == '\0' ? 0 : atoi(gid);

        get_value(s, "GECOS", gecos, sizeof(gecos));
        pw->pw_gecos = (char*) valloc(MAX_VAL_LEN + 1);
        if (gecos[0] == '\0') {
            strcpy(pw->pw_gecos, pw->pw_name);
            pw->pw_gecos[strlen(pw->pw_name)] = '\0';
        }
        else {
            strncpy(pw->pw_gecos, gecos, MAX_VAL_LEN);
            pw->pw_gecos[strlen(gecos)] = '\0';
        }

        get_value(s, "DIR", dir, sizeof(dir));
        pw->pw_dir = (char*) valloc(MAX_VAL_LEN + 1);
        if (dir[0] == '\0') {
            strcpy(pw->pw_dir, "/sys");
            pw->pw_dir[4] = '\0';
        }
        else {
            strncpy(pw->pw_dir, dir, MAX_VAL_LEN);
            pw->pw_dir[strlen(dir)] = '\0';
        }

        get_value(s, "SHELL", shell, sizeof(shell));
        pw->pw_shell = (char*) valloc(MAX_VAL_LEN + 1);
        if (shell[0] == '\0') {
            strcpy(pw->pw_shell, "CLI");
            pw->pw_shell[3] = '\0';
        }
        else {
            strncpy(pw->pw_shell, shell, MAX_VAL_LEN);
            pw->pw_shell[strlen(shell)] = '\0';
        }

        // Debug
        printf("Linea: %s\n", s);
        printf("NAME: '%s'\n", pw->pw_name);
        printf("PASSWORD: %s\n", pw->pw_passwd);
        printf("UID: %d\n", pw->pw_uid);
        printf("GID: %d\n", pw->pw_gid);
        printf("GECOS: %s\n", pw->pw_gecos);
        printf("DIR: %s\n", pw->pw_dir);
        printf("SHELL: %s\n", pw->pw_shell);
        printf("\n");

        break;
    }

    *res = pw;
    printf("ret = %d\n", ret);
    if (ret)
        __set_errno(ret);
    return ret;
}

static int __getpw_a(const char *name, struct passwd *pw, char **buf, size_t *size, struct passwd **res) {
    int ret = 0;
    FILE *file = fopen(_PATH_USERS, "r");
    if (file == NULL) {
        ret = errno;
        goto out;
    }

    while (!(ret = __getpwent_a(file, pw, buf, size, res)) && *res) {
        printf("name %s = not found. Found instead : %s\n", name, (*res)->pw_name);
        if (name && !strcmp(name, (*res)->pw_name)) {
            printf("Found %s\n", name);
            break;
        }
    }

    fclose(file);
    if (!*res) {
        __set_errno(ENOENT);
        ret = ENOENT;
        goto out;
    }

    *res = pw;

out:
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