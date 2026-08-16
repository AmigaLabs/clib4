/*
 * $Id: ftw_nftw.c,v 1.5 2006-01-08 12:04:22 clib4devs Exp $
*/

/*
 * This nftw() implementation builds heavily on existing functionality and could certainly
 * be made more efficient.
 */

#ifndef    _FTW_HEADERS_H
#include "ftw_headers.h"
#endif /* _FTW_HEADERS_H */

/* The directories the traversal is currently inside of. Following symbolic
 * links can lead back into a directory that is already being descended into,
 * and POSIX requires that no directory is visited twice.
 */
struct ancestor {
    const struct ancestor *parent;
    dev_t dev;
    ino_t ino;
};

struct walk_state {
    int (*func)(const char *, const struct stat *, int, struct FTW *);
    int flags;
};

static int
is_ancestor(const struct ancestor *chain, const struct stat *st) {
    while (chain != NULL) {
        if (chain->dev == st->st_dev && chain->ino == st->st_ino)
            return TRUE;

        chain = chain->parent;
    }

    return FALSE;
}

static void
free_names(char **names, int count) {
    int i;

    if (names == NULL)
        return;

    for (i = 0; i < count; i++)
        free(names[i]);

    free(names);
}

/* Read a whole directory into memory, so that its stream can be closed before
 * descending into it: only one directory stream is then ever open, whatever
 * the fd_limit passed to nftw().
 *
 * The "." and ".." entries are left out. readdir() reports them when unix
 * path semantics are in effect, and walking into them would send the
 * traversal round in circles.
 */
static char **
read_names(DIR *dp, int *out_count) {
    char **names;
    struct dirent *de;
    int count = 0;
    int capacity = 32;

    names = malloc(capacity * sizeof(*names));
    if (names == NULL) {
        __set_errno(ENOMEM);
        return (NULL);
    }

    while ((de = readdir(dp)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        if (count == capacity) {
            char **larger = realloc(names, 2 * capacity * sizeof(*names));
            if (larger == NULL) {
                free_names(names, count);
                __set_errno(ENOMEM);
                return (NULL);
            }

            names = larger;
            capacity *= 2;
        }

        names[count] = strdup(de->d_name);
        if (names[count] == NULL) {
            free_names(names, count);
            __set_errno(ENOMEM);
            return (NULL);
        }

        count++;
    }

    (*out_count) = count;

    return (names);
}

static int
walk(const char *path, int level, int base, const struct walk_state *ws,
     const struct ancestor *parents, int *const prune) {
    int result = OK;
    DIR *dp = NULL;
    struct stat st;
    struct ancestor self;
    struct FTW extra_info;
    char **names = NULL;
    char *next_name = NULL;
    char *old_cwd = NULL;
    int count = 0;
    int type;
    int stat_result;
    int old_length;
    int errtmp;
    int i;

    ENTER();

    SHOWSTRING(path);
    SHOWVALUE(level);

    __check_abort();

    if (FLAG_IS_SET(ws->flags, FTW_PHYS))
        stat_result = lstat(path, &st);
    else
        stat_result = stat(path, &st);

    if (stat_result == OK) {
        if (S_ISLNK(st.st_mode)) /* Should only be possible if lstat() is used. */
        {
            type = FTW_SL;
        } else if (S_ISDIR(st.st_mode)) {
            type = FTW_D;
        } else {
            /* Everything that is neither a directory nor a symbolic link is
               reported as a file, sockets and fifos included. */
            type = FTW_F;
        }
    } else {
        if (FLAG_IS_CLEAR(ws->flags, FTW_PHYS) && lstat(path, &st) == OK)
            type = FTW_SLN;    /* Broken link. */
        else
            type = FTW_NS;    /* No Stat */
    }

    /* The root of the walk not being there at all is an error, rather than
       something to report to the callback: nftw() fails with the errno left
       behind by stat(). */
    if (type == FTW_NS && level == 0) {
        result = ERROR;
        goto out;
    }

    extra_info.quit = 0;
    extra_info.base = base;
    extra_info.level = level;

    if (type != FTW_D) {
        result = (*ws->func)(path, &st, type, &extra_info);
        goto out;
    }

    /* A directory that is already being descended into is reported as usual
       but not entered again, as entering it a second time would never end.
       One whose contents cannot be read is reported as FTW_DNR. Either way
       the traversal itself carries on. */
    if (!is_ancestor(parents, &st)) {
        dp = opendir(path);    /* Also takes care of Unix->Amiga pathname conversion. */

        if (dp == NULL)
            type = FTW_DNR;
    }

    if (FLAG_IS_CLEAR(ws->flags, FTW_DEPTH)) {
        result = (*ws->func)(path, &st, type, &extra_info);

        /* POSIX: a non-zero return from the callback ends the traversal at
           once, and its value is what nftw() returns. */
        if (result != OK)
            goto out;

        if (FLAG_IS_SET(extra_info.quit, FTW_SKD)) /* Skip this directory. */
            goto post;
    }

    if (dp == NULL)
        goto post;

    names = read_names(dp, &count);

    errtmp = __get_errno();
    closedir(dp);
    dp = NULL;
    __set_errno(errtmp);

    if (names == NULL) {
        result = ERROR;
        goto out;
    }

    if (FLAG_IS_SET(ws->flags, FTW_CHDIR)) /* Change to directory before traversing. */
    {
        old_cwd = malloc(PATH_MAX);
        if (old_cwd == NULL) {
            __set_errno(ENOMEM);

            result = ERROR;
            goto out;
        }

        getcwd(old_cwd, PATH_MAX);
        chdir(path);
    }

    old_length = strlen(path);

    /* Allocate new for each recursive step to handle extremely long path names. */
    next_name = malloc(old_length + NAME_MAX + 2);
    if (next_name == NULL) {
        __set_errno(ENOMEM);

        result = ERROR;
        goto out;
    }

    strcpy(next_name, path);
    if (old_length > 0 && next_name[old_length - 1] != ':' && next_name[old_length - 1] != '/')
        next_name[old_length++] = '/';

    self.parent = parents;
    self.dev = st.st_dev;
    self.ino = st.st_ino;

    for (i = 0; i < count; i++) {
        strlcpy(&next_name[old_length], names[i], NAME_MAX + 2);

        result = walk(next_name, level + 1, old_length, ws, &self, prune);
        if (result != OK)
            goto out;

        if ((*prune) != 0) {
            (*prune) = 0;
            break;
        }
    }

post:

    if (FLAG_IS_SET(ws->flags, FTW_DEPTH)) {
        /* Post-order: the directory itself is only reported once everything
           below it has been seen. */
        extra_info.quit = 0;
        extra_info.base = base;
        extra_info.level = level;

        result = (*ws->func)(path, &st, (type == FTW_DNR) ? FTW_DNR : FTW_DP, &extra_info);
    }

    if (FLAG_IS_SET(extra_info.quit, FTW_PRUNE)) /* Abort traversal of current directory. */
        (*prune) = 1;

out:

    if (dp != NULL) {
        errtmp = __get_errno();
        closedir(dp);
        __set_errno(errtmp);
    }

    free_names(names, count);
    free(next_name);

    if (old_cwd != NULL) {
        errtmp = __get_errno();

        chdir(old_cwd); /* Restore (pop) old directory. */
        free(old_cwd);

        __set_errno(errtmp);
    }

    RETURN(result);
    return (result);
}

static int
index_of_end_part(const char *path) {
    int result = OK;
    int i;

    i = strlen(path) - 1;
    while (i-- > 0) {
        if (path[i] == '/' || path[i] == ':') {
            result = i + 1;
            break;
        }
    }

    return (result);
}

int
nftw(const char *path, int (*func)(const char *, const struct stat *, int, struct FTW *), int depth, int flags) {
    struct walk_state ws;
    char *base;
    int len;
    int base_index;
    int prune = 0;
    int result = ERROR;

    ENTER();

    SHOWSTRING(path);
    SHOWPOINTER(func);
    SHOWVALUE(depth);
    SHOWVALUE(flags);

    if (path == NULL) {
        SHOWMSG("NULL pathname to nftw().");
        __set_errno(EFAULT);
        goto out;
    }

    if (func == NULL) {
        SHOWMSG("No function supplied.");
        __set_errno(EFAULT);
        goto out;
    }

    /* POSIX calls this parameter fd_limit: it is the number of file
       descriptors nftw() may use, not a limit on how deep it may go. Each
       directory is read and closed before it is descended into, so a single
       descriptor is all this implementation ever needs. */
    if (depth < 0) {
        SHOWMSG("Invalid fd_limit.\n");
        __set_errno(EINVAL);
        goto out;
    }

    if ((flags & ~FTW_ALL_FLAGS) != 0) {
        SHOWMSG("Bogus flags.");

        __set_errno(EINVAL);
        goto out;
    }

    ws.func = func;
    ws.flags = flags;

    /* Make sure the path never ends with '/' unless used to indicate parent directory. */

    len = strlen(path);
    if (len > 1 && path[len - 1] == '/' && path[len - 2] != '/') {
        int error;

        base = strdup(path);
        if (base == NULL) {
            __set_errno(ENOMEM);
            goto out;
        }

        base[len - 1] = '\0';

        base_index = index_of_end_part(base);

        result = walk(base, 0, base_index, &ws, NULL, &prune);

        error = __get_errno();
        free(base);
        __set_errno(error);
    } else {
        base_index = index_of_end_part(path);

        result = walk(path, 0, base_index, &ws, NULL, &prune);
    }

out:

    RETURN(result);
    return (result);
}
