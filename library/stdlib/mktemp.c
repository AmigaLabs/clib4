/*
 * $Id: stdlib_mktemp.c,v 1.8 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <time.h>
#include "../dos.h"

char *
mktemp(char *name_template) {
    struct name_translation_info name_template_nti;
    char *test_name;
    struct Process *this_process;
    APTR old_window_pointer;
    char *result = NULL;
    size_t template_offset;
    size_t template_len;
    size_t name_len;
    size_t offset;
    time_t now = 0;
    ULONG pseudo_random_number;
    BPTR lock;
    size_t i;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(name_template);

    assert(name_template != NULL);

    __check_abort_f(__clib4);

    if (name_template == NULL) {
        SHOWMSG("invalid name template");

        __set_errno(EFAULT);
        goto out;
    }

    this_process = __clib4->self;

    SHOWSTRING(name_template);

    /* So, how long is that name template? */
    name_len = strlen(name_template);
    if (name_len == 0) {
        SHOWMSG("invalid name template");

        __set_errno(EINVAL);
        goto out;
    }

    /* Find out how many trailing 'X' characters there are in
       the template. There should be at least 6. We also want
       to know where to find the first 'X' and how many of the
       'X' characters there are. */
    template_offset = 0;
    template_len = 0;

    for (i = 0; i < name_len; i++) {
        assert(name_len >= (i + 1));
        offset = name_len - (i + 1);
        if (name_template[offset] != 'X')
            break;

        template_offset = offset;
        template_len++;
    }

    SHOWVALUE(template_offset);
    SHOWVALUE(template_len);

    if (template_len == 0) {
        SHOWMSG("invalid name template");

        __set_errno(EINVAL);
        goto out;
    }

    /* Generate a pseudo-random number from the current time and
       the address of the current process. */
    time(&now);

    pseudo_random_number = (ULONG) now + (ULONG) this_process;

    /* Fill the template 'X' characters with letters made up by
       converting the pseudo-random number. */
    for (i = 0; i < template_len; i++) {
        name_template[template_offset + i] = 'A' + (pseudo_random_number % 26);

        /* One more letter taken; if we run out of letters,
           cook up another pseudo-random number. */
        pseudo_random_number = (pseudo_random_number / 26);
        if (pseudo_random_number == 0) {
            time(&now);

            pseudo_random_number = (ULONG) now;
        }
    }

    SHOWSTRING(name_template);

    /* Now check if the name we picked is unique. If not, make another name. */
    while (TRUE) {
        __check_abort_f(__clib4);

        D(("checking '%s'", name_template));

        test_name = name_template;

        /* If necessary, quickly translate the semantics of the file name
		   we cooked up above. */
        if (__clib4->__unix_path_semantics) {
            if (__translate_unix_to_amiga_path_name((char const **) &test_name, &name_template_nti) != 0)
                goto out;

            if (name_template_nti.is_root) {
                __set_errno(EACCES);
                goto out;
            }
        }

        /* Turn off DOS error requesters. */
        old_window_pointer = __set_process_window((APTR) - 1);

        /* Does this object exist already? */
        lock = Lock(test_name, SHARED_LOCK);

        /* Restore DOS requesters. */
        __set_process_window(old_window_pointer);

        if (lock == BZERO) {
            /* If the object does not exist yet then we
             * are finished.
             */
            if (IoErr() == ERROR_OBJECT_NOT_FOUND) {
                result = name_template;
                break;
            }

            /* Looks like a serious error. */
            __set_errno(__translate_io_error_to_errno(IoErr()));
            goto out;
        }

        /* OK, so it exists already. Start over... */
        UnLock(lock);

        /* Change one letter; if that 'overflows', start
           over with 'A' and move on to the next position. */
        for (i = 0; i < template_len; i++) {
            name_template[template_offset + i]++;
            if (name_template[template_offset + i] <= 'Z')
                break;

            name_template[template_offset + i] = 'A';
        }
    }

    SHOWSTRING(name_template);

out:

    RETURN(result);
    return (result);
}
