/*
 * $Id: unistd_translateu2a.c,v 1.13 2015-06-26 11:22:00 clib4devs Exp $
*/

/*#define DEBUG*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/*
 * The following patterns must translate properly:
 *
 *		foo/
 *		///
 *		foo//bar
 *		foo//bar//baz
 *		./foo
 *		././foo
 *		foo/./baz
 *		foo/./bar/./baz
 *		foo/./././bar
 *		foo/.
 *		/.
 *		/tmp
 *		/tmp/foo
 *		/dev/null
 *		/dev/null/foo
 *		/dev/nullX
 *		/foo
 *		/foo/
 *		/foo/bar
 *		/foo/bar/baz
 *		foo/../bar
 *		foo/bar/../../baz
 *		foo/bar/..
 *		../foo
 *		../../foo
 *		.
 *		..
 */

// extern int __search_expand_command_path(char const **name_ptr, char *replacement_buffer, size_t replacement_buffer_size);

int
__translate_unix_to_amiga_path_name(char const **name_ptr, struct name_translation_info *nti) {
    int result = ERROR;
    char *name;

    /*PUSHDEBUGLEVEL(2);*/

    assert(name_ptr != NULL && (*name_ptr) != NULL && nti != NULL);

    name = (char *) (*name_ptr);

    nti->is_root = FALSE;

    /* If this is not an absolute Amiga path name, which we will accept "as is",
       perform the translation and substitution. */
    if (strchr(name, ':') == NULL) {
        char home_dir_name[sizeof(nti->substitute)];
        char volume_name[sizeof(nti->substitute)];
        size_t volume_name_len;
        char *replace = nti->substitute;
        size_t len, i, j;

        /* Check if the name would become too long to handle. */
        len = strlen(name);
        if (len >= sizeof(nti->substitute)) {
            D(("path name '%s' is too long (%ld characters total; maximum is %ld)!", name, len,  sizeof(nti->substitute) - 1));
            __set_errno(ENAMETOOLONG);
            goto out;
        }

        D(("initial name = '%s'", name));

        /* If the first character of the path name is a tilde, then the
           path should be relative to the user's home directory. */
        if (name[0] == '~' && 0) {
            LONG home_dir_name_len;

            /* If the HOME environment variable is set, it is supposed to refer
               to the name of the directory that will replace the tilde
               character. This should be a Unix-style path name and not
               something that makes life harder by being an Amiga path name.
               Absolute Amiga path names are ignore, as is the absence of
               the HOME variable. */
            home_dir_name_len = GetVar("HOME", (STRPTR) home_dir_name, (LONG)
            sizeof(home_dir_name), 0);
            if (home_dir_name_len <= 0 || strchr(home_dir_name, ':') != NULL) {
                /* Nothing useful here. Replace the tilde with the
                   current directory. */
                strcpy(home_dir_name, ".");
                home_dir_name_len = 1;
            }

            /* Will this fit? */
            if (len + home_dir_name_len >= sizeof(nti->substitute)) {
                __set_errno(ENAMETOOLONG);
                goto out;
            }

            /* Add a path separator unless it's already there. */
            if (home_dir_name[home_dir_name_len - 1] != '/')
                home_dir_name[home_dir_name_len++] = '/';

            /* Skip the tilde and any path name separators that may follow it. */
            name++;
            while ((*name) == '/')
                name++;

            /* Now combine the home directory name with the remainder
               of the path name. */
            strcpy(&home_dir_name[home_dir_name_len], name);
            name = home_dir_name;
        }
#if 0
        else {
            
            /* If input string is a command, we can use the system path to expand.
               This should not conflict with previous implements, because the only
               diverging situation is where it would previously return a non-existing
               path to the current directory. */

            /* Response : This is not true. In cases, where the application is trying
                to determine if an object in the current dir exists, it can potentially
                give the wrong answer, if an object with immediate address on the 
                search path exists. For instance:
                
                Current dir : /work/foo
                stat on : bar
                On path: bar

                This can possibly result in a stat returning the address

                    /work/foo/bar

                which doesn't exist (awennermark).
            */

            if(strchr(name, '/') == NULL) {
                if(__search_expand_command_path((const char **) &name, nti->substitute, sizeof(nti->substitute)) == 0) {
                    SHOWMSG("Successfully expanded command string using system paths.\n");
                    result = OK;
                    name = replace;
                    // printf("name : %s\n", name);
                    (*name_ptr) = name;
                    goto out;
                }
            }
        }
#endif

#if 0
        /* Prepend an absolute path to the name, if such a path was previously set
           as the current directory. */

        /* Response : This is not good. We might be dealing with a command name (as in popen),
            which means, that the tagging-on of any path (cwd or otherwise) to the command
            name could potentially break the call. Even if this leaves the result of this call
            with a non-absolute path, it will still work, in case the sought-after object is in fact
            found in the current directory. So there are no benefits in tagging on the current
            path - it will not add anything to the functionality of the enterprise. (awennermark) */

        if (__translate_relative_path_name((const char **) &name, nti->substitute, sizeof(nti->substitute)) < 0) {
            SHOWMSG("relative path name could not be worked into the pattern");
            goto out;
        }
#endif
        /* If we wound up with the expanded home directory name,
           put it into the substitution string. */
        if (name == home_dir_name) {
            strcpy(replace, home_dir_name);
            name = replace;
        }

        /* If the name was replaced, update the string length cached. */
        if (name != (*name_ptr)) {
            D(("name after relative path replacement = '%s'", name));

            len = strlen(name);
        }

        /* If there are neighboring slashes, strip all but one
           ('foo//bar' -> 'foo/bar'). The "//" pattern in a Unix
           file name is apparently harmless, but on AmigaDOS it
           has a very definite meaning. */
        if (len > 2) {
            BOOL have_double_slash = FALSE;

            for (i = 0; i < len - 1; i++) {
                if (name[i] == '/' && name[i + 1] == '/') {
                    have_double_slash = TRUE;
                    break;
                }
            }

            if (have_double_slash) {
                BOOL have_slash;
                char c;

                have_slash = FALSE;

                for (i = j = 0; i < len; i++) {
                    c = name[i];

                    if (c == '/') {
                        if (NOT have_slash)
                            replace[j++] = c;

                        have_slash = TRUE;
                    } else {
                        replace[j++] = c;

                        have_slash = FALSE;
                    }
                }

                name = replace;

                len = j;

                name[len] = '\0';
                D(("name = '%s' (line %ld)", name, __LINE__));
            }
        }

        /* If there are any, strip the trailing slashes ('foo/' -> 'foo'). A
           leading '/' must be preserved, though ('///' -> '/'). */
        if (len > 1) {
            size_t num_trailing_slashes = 0;

            while ((num_trailing_slashes < len - 1) && (name[len - (num_trailing_slashes + 1)] == '/'))
                num_trailing_slashes++;

            if (num_trailing_slashes > 0) {
                len -= num_trailing_slashes;

                if (name != replace) {
                    memmove(replace, name, len);
                    name = replace;
                }

                name[len] = '\0';
                D(("name = '%s' (line %ld)", name, __LINE__));
            }
        }

        /* Ditch all leading './' ('./foo' -> 'foo'). */
        while (len > 2 && name[0] == '.' && name[1] == '/') {
            name += 2;
            len -= 2;

            D(("name = '%s' (line %ld)", name, __LINE__));
        }

        /* Ditch all embedded '/./' ('foo/./bar' -> 'foo/bar', 'foo/././bar' -> 'foo/bar'). */
        if (len > 2) {
            BOOL have_slash_dot_slash = FALSE;

            for (i = j = 0; i < len - 2; i++) {
                if (name[i] == '/' && name[i + 1] == '.' && name[i + 2] == '/') {
                    have_slash_dot_slash = TRUE;
                    break;
                }
            }

            if (have_slash_dot_slash) {
                for (i = j = 0; i < len; i++) {
                    while (i < len - 2 && name[i] == '/' && name[i + 1] == '.' && name[i + 2] == '/')
                        i += 2;

                    if (i < len)
                        replace[j++] = name[i];
                }

                len = j;

                name = replace;

                name[len] = '\0';
                D(("name = '%s' (line %ld)", name, __LINE__));
            }
        }

        /* Special case: the path name may end with "/." signifying that the
           directory itself is requested ('foo/.' -> 'foo'). */
        if (len >= 2 && strncmp(&name[len - 2], "/.", 2) == SAME) {
            /* If it's just '/.' then it's really '/'. */
            if (len == 2) {
                strcpy(replace, "/");
                name = replace;

                len = 1;
            } else {
                if (name != replace) {
                    memmove(replace, name, len - 2);
                    name = replace;
                }

                len -= 2;
            }

            name[len] = '\0';
            D(("name = '%s' (line %ld)", name, __LINE__));
        }

        /* Check for absolute path. */
        if (name[0] == '/') {
            if (len == 1) {
                SHOWMSG("this is the root directory");

                nti->is_root = TRUE;
            }

            /* Ok, so this is an absolute path. We begin by checking
               for a few special cases, the first being a reference
               to "/tmp". */
            if ((strncmp(name, "/tmp", 4) == SAME) && (name[4] == '/' || len == 4)) {
                if (name[4] == '/') {
                    /* Convert "/tmp/foo" to "T:foo". */
                    memmove(&replace[2], &name[5], len - 5);
                    memmove(replace, "T:", 2);

                    len -= 3;
                } else {
                    /* Convert "/tmp" to "T:". */
                    strcpy(replace, "T:");

                    len = 2;
                }

                name = replace;

                name[len] = '\0';
                D(("name = '%s' (line %ld)", name, __LINE__));
            } else if ((strncmp(name, "/dev/null", 9)) == SAME && (len == 9 || name[9] == '/')) {
                strcpy(replace, "NIL:");
                name = replace;

                len = 4;

                name[len] = '\0';
                D(("name = '%s' (line %ld)", name, __LINE__));
            } else if (
                (strncmp(name, "/dev/random", 11)) == SAME && (len == 11 || name[11] == '/') ||
                (strncmp(name, "/dev/urandom", 12)) == SAME && (len == 12 || name[12] == '/')
            ) {
                strcpy(replace, "RANDOM:");
                name = replace;

                len = 7;

                name[len] = '\0';
                D(("name = '%s' (line %ld)", name, __LINE__));
            } else {
                size_t path_name_start = 0;

                volume_name_len = 0;

                /* Find out how long the first component
                   of the absolute path is. */
                for (i = 1; i <= len; i++) {
                    if (i == len || name[i] == '/') {
                        volume_name_len = i - 1;

                        /* Is there anything following the path name? */
                        if (i < len)
                            path_name_start = i + 1;

                        break;
                    }
                }

                /* Copy the first component and attach a colon. "/foo" becomes "foo:". */
                memmove(replace, &name[1], volume_name_len);
                replace[volume_name_len++] = ':';

                /* Now add the finishing touches. "/foo/bar" finally
                   becomes "foo:bar" and "/foo" becomes "foo:". */
                if (path_name_start > 0) {
                    memmove(&replace[volume_name_len], &name[path_name_start], len - path_name_start);

                    len--;
                }

                name = replace;

                name[len] = '\0';
                D(("name = '%s' (line %ld)", name, __LINE__));
            }
        }

        /* Extract and remove the volume name from the path. We
           are going to need it later. */
        volume_name_len = 0;

        for (i = 0; i < len; i++) {
            if (name[i] == ':') {
                /* Look for extra colon characters embedded in the name
                   (as in "foo/bar:baz") which really don't belong here. */
                for (j = 0; j < i; j++) {
                    if (name[j] == '/') {
                        D(("name '%s' still contains colon characters", name));

                        __set_errno(EINVAL); /* invalid name */
                        goto out;
                    }
                }

                volume_name_len = i + 1;

                memmove(volume_name, name, volume_name_len);

                name += volume_name_len;
                len -= volume_name_len;

                D(("name = '%s' (line %ld)", name, __LINE__));

                break;
            }
        }

        /* Look for extra colon characters embedded in the name
           (as in "foo:bar:baz") which really don't belong here. */
        for (i = 0; i < len; i++) {
            if (name[i] == ':') {
                D(("name '%s' still contains colon characters", name));

                __set_errno(EINVAL); /* invalid name */
                goto out;
            }
        }

        /* Now parse the path name and replace all embedded '..' with
           the AmigaDOS counterparts ('foo/../bar' -> 'foo//bar'). */
        if (len > 3) {
            BOOL have_slash_dot_dot_slash = FALSE;

            for (i = j = 0; i < len - 3; i++) {
                if (name[i] == '/' && name[i + 1] == '.' && name[i + 2] == '.' && name[i + 3] == '/') {
                    have_slash_dot_dot_slash = TRUE;
                    break;
                }
            }

            if (have_slash_dot_dot_slash) {
                for (i = j = 0; i < len; i++) {
                    if (i < len - 3 && name[i] == '/' && name[i + 1] == '.' && name[i + 2] == '.' &&
                        name[i + 3] == '/') {
                        replace[j++] = '/';
                        i += 3;
                    }

                    replace[j++] = name[i];
                }

                len = j;

                name = replace;

                name[len] = '\0';
                D(("name = '%s' (line %ld)", name, __LINE__));
            }
        }

        /* Translate a trailing '/..' to '//' */
        if (len >= 3 && strncmp(&name[len - 3], "/..", 3) == SAME) {
            if (name != replace) {
                memmove(replace, name, len - 2);
                name = replace;
            }

            len -= 2;

            name[len++] = '/';

            name[len] = '\0';
            D(("name = '%s' (line %ld)", name, __LINE__));
        }

        /* Translate a leading '../' ('../foo' -> '/foo') */
        if (len >= 3 && strncmp(name, "../", 3) == SAME) {
            memmove(replace, &name[2], len - 2);
            name = replace;

            len -= 2;

            name[len] = '\0';
            D(("name = '%s' (line %ld)", name, __LINE__));
        }

        /* Translate the '..' ('..' -> '/') */
        if (len == 2 && strncmp(name, "..", 2) == SAME) {
            strcpy(replace, "/");
            name = replace;

            len = 1;

            D(("name = '%s' (line %ld)", name, __LINE__));
        }

        /* Translate the '.' ('.' -> '') */
        if (len == 1 && name[0] == '.') {
            strcpy(replace, "");
            name = replace;

            len = 0;

            D(("name = '%s' (line %ld)", name, __LINE__));
        }

        /* Now put it all together again. */
        if (volume_name_len > 0) {
            memmove(&replace[volume_name_len], name, len);
            memmove(replace, volume_name, volume_name_len);

            len += volume_name_len;

            name = replace;

            name[len] = '\0';
            D(("name = '%s' (line %ld)", name, __LINE__));
        }

        /* Reduce any "//" embedded in the name, if necessary. */
        if (len > 1) {
            BOOL have_double_slash = FALSE;

            for (i = 0; i < len - 1; i++) {
                if (name[i] == '/' && name[i + 1] == '/') {
                    have_double_slash = TRUE;
                    break;
                }
            }

            if (have_double_slash) {
                if (name != replace) {
                    memmove(replace, name, len);
                    name = replace;
                }

                len = __strip_double_slash(name, len);

                D(("name = '%s' (line %ld)", name, __LINE__));
            }
        }

        /* The following is somewhat controversial. It assumes that what comes out
           as a path name that ends with ":/" should translate into the root
           directory. This would not be the case for the names of assignments,
           though, for which the "/" suffix would actually do something useful. */
        if (len >= 2 && strncmp(&name[len - 2], ":/", 2) == SAME)
            nti->is_root = TRUE;
    }

    nti->original_name = (char *) (*name_ptr);
    (*name_ptr) = name;

    D(("translated '%s' to '%s'", nti->original_name, name));

    result = OK;

out:

    /*POPDEBUGLEVEL();*/

    return (result);
}
