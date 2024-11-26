/*
 * $Id: unistd_wildcard_expand.c,v 1.17 2006-04-05 08:39:46 clib4devs Exp $
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#ifndef CLIB_ALIB_PROTOS_H
#include <clib/alib_protos.h>
#endif /* CLIB_ALIB_PROTOS_H */

#define ap_Buf ap_Buffer

static size_t quote_vector_size;
static UBYTE *quote_vector;

int __wildcard_quote_parameter(unsigned int parameter) {
    size_t num_bytes;
    int result = ERROR;

    /* Can we mark this parameter as quoted or do we need more
       room in the buffer? */
    num_bytes = (parameter + 7) / 8;
    if (num_bytes >= quote_vector_size) {
        size_t new_quote_vector_size = quote_vector_size + 8;
        UBYTE *new_quote_vector;

        /* Allocate a larger buffer. */
        new_quote_vector = realloc(quote_vector, new_quote_vector_size);
        if (new_quote_vector == NULL)
            goto out;

        /* Set the bits in the new buffer portion to 0. */
        memset(&new_quote_vector[quote_vector_size], 0, new_quote_vector_size - quote_vector_size);

        quote_vector_size = new_quote_vector_size;
        quote_vector = new_quote_vector;
    }

    assert(quote_vector != NULL);
    assert(num_bytes < quote_vector_size);

    quote_vector[parameter / 8] |= 1 << (7 - (parameter % 8));

    result = OK;

out:

    return (result);
}

CLIB_DESTRUCTOR(__wildcard_expand_exit) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->anchor != NULL) {
        MatchEnd(__clib4->anchor);
        FreeDosObject(DOS_ANCHORPATH, __clib4->anchor);
        __clib4->anchor = NULL;
    }

    LEAVE();
}

struct name_node {
    struct MinNode nn_min_node;
    char *nn_name;
    BOOL nn_wild;
};

static int
compare(const char **a, const char **b) {
    return (strcmp((*a), (*b)));
}

int __wildcard_expand_init(void) {
    APTR old_window_pointer;
    struct AnchorPath *ap = NULL;
    struct MinList argument_list;
    size_t argument_list_size;
    BOOL found_pattern = FALSE;
    BOOL match_made;
    char *replacement_arg = NULL;
    struct name_node *node;
    int argc;
    char **argv;
    int error;
    int i;
    struct _clib4 *__clib4 = __CLIB4;

    /* Disable dos.library requesters during pattern matching below. We
       do this so early in order to make it easier to reset the window
       pointer in the cleanup code. */
    old_window_pointer = __set_process_window((APTR) - 1);

    /* No work to be done? */
    if (__clib4->__argc == 0 || __clib4->__argv == NULL) {
        error = OK;
        goto out;
    }

    argc = __clib4->__argc;
    argv = __clib4->__argv;

    ap = AllocDosObjectTags(DOS_ANCHORPATH,
                            ADO_Strlen, 2 * MAXPATHLEN,
                            ADO_Mask, (__clib4->__check_abort_enabled) ? __clib4->__break_signal_mask : 0,
                            TAG_END);

    if (ap == NULL) {
        error = ENOMEM;
        goto out;
    }

    /* This may have to be cleaned up later. */
    __clib4->anchor = ap;

    /* The argument list will go in here. */
    NewList((struct List *) &argument_list);
    argument_list_size = 0;

    for (i = 0; i < argc; i++) {
        match_made = FALSE;

        /* Check if the string is quoted. Quoted strings are never expanded. */
        if (i > 0 && (quote_vector == NULL || (quote_vector[i / 8] & (1 << (7 - (i % 8)))) == 0)) {
            size_t arg_len, star_count, j;
            char last_c;
            char *arg;

            arg = argv[i];

            /* Check if there are any asterisks in the argument string.
               These will be replaced by "#?", if possible. If a backtick
               precedes the '*' then it is assumed to be an escaped
               wildcard pattern character which has to be used "as is". */
            arg_len = strlen(arg);

            star_count = 0;
            last_c = '\0';

            for (j = 0; j < arg_len; j++) {
                if (arg[j] == '*' && last_c != '\'')
                    star_count++;

                last_c = arg[j];
            }

            /* Should we try to create a replacement string? */
            if (star_count > 0) {
                size_t k;

                if (replacement_arg != NULL)
                    __free_r(__clib4, replacement_arg);

                replacement_arg = __malloc_r(__clib4, arg_len + star_count + 1);
                if (replacement_arg == NULL) {
                    error = ENOMEM;
                    goto out;
                }

                last_c = '\0';

                /* Replace each "*" with "#?". */
                for (j = k = 0; j < arg_len; j++) {
                    if (arg[j] == '*' && last_c != '\'') {
                        replacement_arg[k++] = '#';
                        replacement_arg[k++] = '?';
                    } else {
                        replacement_arg[k++] = arg[j];
                    }

                    last_c = arg[j];
                }

                replacement_arg[k] = '\0';

                arg = replacement_arg;
            }

            /* Does this string contain a wildcard pattern? */
            if (ParsePatternNoCase(arg, ap->ap_Buf, 2 * MAXPATHLEN) > 0) {
                BOOL is_first = TRUE;
                LONG rc;

                rc = MatchFirst(arg, ap);

                while (TRUE) {
                    /* Got a break signal? */
                    if (rc == ERROR_BREAK) {
                        __set_process_window(old_window_pointer);

                        SetSignal(__clib4->__break_signal_mask, __clib4->__break_signal_mask);
                        __check_abort_f(__clib4);

                        old_window_pointer = __set_process_window((APTR) - 1);

                        /* If we ever arrive here, retry the previous match. */
                        if (is_first) {
                            MatchEnd(ap);

                            rc = MatchFirst(arg, ap);
                        } else {
                            rc = MatchNext(ap);
                        }

                        continue;
                    } else if (rc == ERROR_NO_MORE_ENTRIES) {
                        /* End of the line. */
                        break;
                    } else if (rc != OK) {
                        /* Some error occurred. */
                        error = EIO;
                        break;
                    }

                    /* Allocate another node for the expanded parameter. */
                    node = malloc(sizeof(*node) + strlen(ap->ap_Buf) + 1);
                    if (node == NULL) {
                        error = ENOMEM;
                        goto out;
                    }

                    /* Remember that this name ended up here due to pattern matching. */
                    node->nn_name = (char *) (node + 1);
                    node->nn_wild = TRUE;

                    strcpy(node->nn_name, ap->ap_Buf);

                    AddTail((struct List *) &argument_list, (struct Node *) node);
                    argument_list_size++;

                    rc = MatchNext(ap);

                    is_first = FALSE;

                    /* Remember that we found a wildcard pattern among the arguments. */
                    match_made = found_pattern = TRUE;
                }

                MatchEnd(ap);
            }
        }

        if (NOT match_made) {
            /* Just remember this argument as is. */
            node = malloc(sizeof(*node));
            if (node == NULL) {
                error = ENOMEM;
                goto out;
            }

            node->nn_name = argv[i];
            node->nn_wild = FALSE;

            AddTail((struct List *) &argument_list, (struct Node *) node);
            argument_list_size++;
        }
    }

    /* If there were wildcard patterns among the command line parameters,
     * work them into the argument vector.
     */
    if (found_pattern) {
        size_t last_wild, position;
        char **table;

        /* This will hold the new arguments. */
        table = malloc(sizeof(*table) * (argument_list_size + 1));
        if (table == NULL) {
            error = ENOMEM;
            goto out;
        }

        __clib4->__argc = argument_list_size;
        __clib4->__argv = table;

        /* Fill in the table, sorting the wildcard matches. */
        last_wild = 0;

        for (node = (struct name_node *) argument_list.mlh_Head, position = 0;
             node->nn_min_node.mln_Succ != NULL;
             node = (struct name_node *) node->nn_min_node.mln_Succ, position++) {
            if (node->nn_wild) {
                /* Remember where we found the last parameter that was
                   added due to pattern matching. */
                if (last_wild == 0)
                    last_wild = position;
            } else {
                /* This is not a parameter which was added due to pattern
                   matching. But if we added one before, we will want to
                   sort all these parameters alphabetically. */
                if (last_wild != 0) {
                    size_t num_elements;

                    assert(position > last_wild);
                    assert(last_wild < argument_list_size);

                    /* How many parameters would have to be sorted? */
                    num_elements = position - last_wild;
                    if (num_elements > 1)
                        qsort(&table[last_wild], num_elements, sizeof(*table),
                              (int (*)(const void *, const void *)) compare);

                    last_wild = 0;
                }
            }

            assert(position < argument_list_size);

            table[position] = node->nn_name;
        }

        /* If necessary, take care of the last entries in the list. */
        if (last_wild != 0) {
            size_t num_elements;

            assert(position > last_wild);
            assert(last_wild < argument_list_size);

            /* How many parameters would have to be sorted? */
            num_elements = position - last_wild;
            if (num_elements > 1)
                qsort(&table[last_wild], num_elements, sizeof(*table), (int (*)(const void *, const void *)) compare);
        }

        assert(position == argument_list_size);

        /* This terminates the table. */
        table[position] = NULL;
    } else {
        /* Throw the contents of the list away. There is nothing worth keeping. */
        while ((node = (struct name_node *) RemHead((struct List *) &argument_list)) != NULL)
            __free_r(__clib4, node);
    }

    error = OK;

out:

    __set_process_window(old_window_pointer);

    if (ap != NULL) {
        MatchEnd(ap);

        FreeDosObject(DOS_ANCHORPATH, __clib4->anchor);
    }

    __clib4->anchor = NULL;

    if (error != OK) {
        __set_errno_r(__clib4, error);

        perror(__clib4->__argv[0]);
        abort();
    }

    if (replacement_arg != NULL)
        __free_r(__clib4, replacement_arg);

    if (quote_vector != NULL) {
        __free_r(__clib4, quote_vector);
        quote_vector = NULL;
    }

    quote_vector_size = 0;

    return (error);
}
