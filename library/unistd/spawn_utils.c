/*
* $Id: unistd_spawn_utils.c,v 1.0 2025-01-06 11:04:00 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "spawn_utils.h"

/* Scan a string for characters which may require that the string
   should be quoted */
BOOL
string_needs_quoting(const char *string, size_t len) {
    BOOL result = FALSE;
    size_t i;
    char c;

    for (i = 0; i < len; i++) {
        c = (*string++);
        if (c == ' ' || ((unsigned char) c) == 0xA0 || c == '\t' || c == '\n' || c == '\"') {
            result = TRUE;
            break;
        }
    }

    return (result);
}

/* Put together an argument string from a list of individual
   components, quoting characters, escape characters and
   separator characters. You're supposed to have enough memory
   reserved for the whole string to fit */
void
build_arg_string(char *const argv[], char *arg_string) {
    BOOL first_char = TRUE;
    size_t i, j, len;
    char *s;

    /* The first argv[] element is skipped; it does not contain part of
	   the command line but holds the name of the program to be run. */
    for (i = 1; argv[i] != NULL; i++) {
        s = (char *) argv[i];

        len = strlen(s);
        if (len > 0) {
            if (first_char)
                first_char = FALSE;
            else
                (*arg_string++) = ' ';

            if ((*s) != '\"' && string_needs_quoting(s, len)) {
                (*arg_string++) = '\"';

                for (j = 0; j < len; j++) {
                    if (s[j] == '\"' || s[j] == '*') {
                        (*arg_string++) = '*';
                        (*arg_string++) = s[j];
                    } else if (s[j] == '\n') {
                        (*arg_string++) = '*';
                        (*arg_string++) = 'N';
                    } else {
                        (*arg_string++) = s[j];
                    }
                }

                (*arg_string++) = '\"';
            } else {
                memcpy(arg_string, s, len);
                arg_string += len;
            }
        }
    }
}

/* Scan the string, looking for characters which need to be
   escape with a '*' if that string is to be quoted and the
   contents should remain in the same form */
size_t
count_extra_escape_chars(const char *string, size_t len) {
    size_t count = 0;
    size_t i;
    char c;

    for (i = 0; i < len; i++) {
        c = (*string++);
        if (c == '\"' || c == '*' || c == '\n')
            count++;
    }

    return (count);
}

size_t
get_arg_string_length(char *const argv[]) {
    size_t result = 0;
    size_t i, len = 0;
    char *s;

    /* The first argv[] element is skipped; it does not contain part of
	   the command line but holds the name of the program to be run. */
    for (i = 1; argv[i] != NULL; i++) {
        s = (char *) argv[i];

        len = strlen(s);
        if (len > 0) {
            if ((*s) != '\"') {
                if (string_needs_quoting(s, len))
                    len += 1 + count_extra_escape_chars(s, len) + 1;
            }

            if (result == 0)
                result = len;
            else
                result = result + 1 + len;
        }
    }

    return (result);
}

/* -----------------------------------------------------------------------
 * FD-inheritance helpers shared by spawnvpe, spawnv, and popen.
 * ----------------------------------------------------------------------- */

/*
 * Resolve the canonical DefaultFile for an fd, following the alias chain
 * to the original (the fd that actually owns the BPTR).
 */
static BPTR
__get_resolved_bptr(struct _clib4 *__clib4, int fd_num)
{
    struct fd *fd;

    if (fd_num < 0 || fd_num >= __clib4->__num_fd)
        return BZERO;
    fd = __clib4->__fd[fd_num];
    if (fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE))
        return BZERO;
    if (fd->fd_Original != NULL)
        fd = fd->fd_Original;
    return fd->fd_DefaultFile;
}

/*
 * Build the fd-inheritance spec string for a child process.
 *
 * For each open fd >= 3 that should be inherited (not CLOEXEC, not socket,
 * not directory), encode it as:
 *   "fd_num:flags:handle_or_token;"
 *
 * If fhin/fhout/fherr are >= 0, fds that share the same underlying BPTR
 * as those are stored as symbolic tokens (STDIN/STDOUT/STDERR) so the child
 * can use Input()/Output()/ErrorOutput() — required for PIPE: devices.
 * Pass -1 for fhin/fhout/fherr when the caller does not redirect stdio.
 *
 * Returns a malloc'd string, or NULL if no fds need inheritance.
 */
char *
build_fd_inherit_spec(struct _clib4 *__clib4, int fhin, int fhout, int fherr)
{
    size_t required = 1;
    int count = 0;
    char *out;
    BPTR fhin_bptr  = (fhin  >= 0) ? __get_resolved_bptr(__clib4, fhin)  : BZERO;
    BPTR fhout_bptr = (fhout >= 0) ? __get_resolved_bptr(__clib4, fhout) : BZERO;
    BPTR fherr_bptr = (fherr >= 0) ? __get_resolved_bptr(__clib4, fherr) : BZERO;
    int i;

    for (i = 3; i < __clib4->__num_fd; i++) {
        struct fd *fd = __clib4->__fd[i];

        if (i == fhin || i == fhout || i == fherr)
            continue;
        if (fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE))
            continue;
        if (FLAG_IS_SET(fd->fd_Flags, FDF_CLOEXEC))
            continue;
        if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET))
            continue;
        if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_DIRECTORY | FDF_PATH_ONLY))
            continue;
        if (fd->fd_Action != __fd_hook_entry)
            continue;
        if (fd->fd_DefaultFile == BZERO)
            continue;

        required += 48;
        count++;
    }

    if (count == 0)
        return NULL;

    out = malloc(required);
    if (out == NULL)
        return NULL;
    out[0] = '\0';

    for (i = 3; i < __clib4->__num_fd; i++) {
        struct fd *fd = __clib4->__fd[i];
        const char *alias_token;
        BPTR fd_bptr;
        int written;
        size_t used;

        if (i == fhin || i == fhout || i == fherr)
            continue;
        if (fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE))
            continue;
        if (FLAG_IS_SET(fd->fd_Flags, FDF_CLOEXEC))
            continue;
        if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET))
            continue;
        if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_DIRECTORY | FDF_PATH_ONLY))
            continue;
        if (fd->fd_Action != __fd_hook_entry)
            continue;
        if (fd->fd_DefaultFile == BZERO)
            continue;

        /*
         * If this fd shares the same underlying BPTR as fhin/fhout/fherr,
         * store a symbolic token so the child uses the process-level
         * Input()/Output()/ErrorOutput() handle instead of a DupFileHandle
         * copy.  For AmigaOS4 PIPE: devices, DupFileHandle creates a new
         * independent (disconnected) pipe connection — the child would
         * never receive data written to the original pipe.
         */
        fd_bptr = fd->fd_DefaultFile;
        if (fd->fd_Original != NULL)
            fd_bptr = fd->fd_Original->fd_DefaultFile;

        alias_token = NULL;
        if (fhin_bptr != BZERO && fd_bptr == fhin_bptr)
            alias_token = "STDIN";
        else if (fhout_bptr != BZERO && fd_bptr == fhout_bptr)
            alias_token = "STDOUT";
        else if (fherr_bptr != BZERO && fd_bptr == fherr_bptr)
            alias_token = "STDERR";

        if (alias_token != NULL) {
            used = strlen(out);
            written = snprintf(out + used, required - used, "%d:%lu:%s;", i,
                               (unsigned long) fd->fd_Flags, alias_token);
            if (written < 0 || (size_t) written >= (required - used)) {
                free(out);
                return NULL;
            }
            continue;
        }

        {
            BPTR child_handle = DupFileHandle(fd->fd_DefaultFile);
            if (child_handle == BZERO)
                continue;

            used = strlen(out);
            written = snprintf(out + used, required - used, "%d:%lu:%lx;", i,
                               (unsigned long) fd->fd_Flags,
                               (unsigned long) child_handle);
            if (written < 0 || (size_t) written >= (required - used)) {
                Close(child_handle);
                free(out);
                return NULL;
            }
        }
    }

    return out;
}

/*
 * Close the DupFileHandle'd BPTRs stored in an fd-inheritance spec,
 * skipping symbolic tokens (STDIN/STDOUT/STDERR).
 */
void
close_fd_inherit_spec_handles(char *spec)
{
    char *copy;
    char *entry;

    if (spec == NULL || spec[0] == '\0')
        return;

    copy = strdup(spec);
    if (copy == NULL)
        return;

    entry = strtok(copy, ";");
    while (entry != NULL) {
        char *flags_str = strchr(entry, ':');
        if (flags_str != NULL) {
            *flags_str++ = '\0';
            char *handle_str = strchr(flags_str, ':');
            if (handle_str != NULL) {
                BPTR handle;
                *handle_str++ = '\0';
                /* Symbolic tokens are not DupFileHandle'd BPTRs; skip them. */
                if (strcmp(handle_str, "STDIN")  == 0 ||
                    strcmp(handle_str, "STDOUT") == 0 ||
                    strcmp(handle_str, "STDERR") == 0) {
                    entry = strtok(NULL, ";");
                    continue;
                }
                handle = (BPTR) strtoul(handle_str, NULL, 16);
                if (handle != BZERO)
                    Close(handle);
            }
        }
        entry = strtok(NULL, ";");
    }

    free(copy);
}