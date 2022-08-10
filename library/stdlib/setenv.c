/*
 * $Id: stdlib_setenv.c,v 1.12 2006-09-25 14:12:15 clib2devs Exp $
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

struct LocalVariable {
    struct LocalVariable *lv_Next;
    char *lv_Name;
};

static struct LocalVariable *__lv_root;

CLIB_DESTRUCTOR(__setenv_exit) {
    ENTER();

    /* Now for the local variables that may still be set. */
    if (__lv_root != NULL)
    {
        do {
            D(("deleting variable '%s'", __lv_root->lv_Name));
            DeleteVar(__lv_root->lv_Name, 0);
        } while ((__lv_root = __lv_root->lv_Next) != NULL);
    }

    LEAVE();
}

int
setenv(const char *original_name, const char *original_value, int overwrite) {
    const char *name = original_name;
    const char *value = original_value;
    char *name_copy = NULL;
    struct LocalVariable *lv = NULL;
    struct LocalVar *found;
    int status;
    int result = ERROR;
    size_t i;

    ENTER();

    SHOWSTRING(original_name);
    SHOWSTRING(original_value);
    SHOWVALUE(overwrite);

    assert(original_name != NULL || original_value != NULL);

    __check_abort();

    if (original_name == NULL && original_value == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    if (name != NULL) {
        for (i = 0; i < strlen(name); i++) {
            if (name[i] == '=') {
                name_copy = malloc(i + 1);
                if (name_copy == NULL) {
                    SHOWMSG("could not create copy of name");
                    goto out;
                }

                memmove(name_copy, name, i);
                name_copy[i] = '\0';

                value = &name[i + 1];
                name = name_copy;
                break;
            }
        }
    }
    else if (value != NULL) /* && name == NULL */
    {
        for (i = 0; i < strlen(value); i++) {
            if (value[i] == '=') {
                name_copy = malloc(i + 1);
                if (name_copy == NULL) {
                    SHOWMSG("could not create copy of name");
                    goto out;
                }

                memmove(name_copy, value, i);
                name_copy[i] = '\0';

                name = name_copy;
                value = &value[i + 1];
                break;
            }
        }
    }

    if (name == NULL || name[0] == '\0' || value == NULL) {
        SHOWMSG("invalid name");

        __set_errno(EINVAL);
        goto out;
    }

    if (NOT overwrite) {
        char buffer[10];

        status = GetVar((STRPTR) name, buffer, sizeof(buffer), 0);
        if (status != -1) {
            SHOWMSG("variable already exists; leaving...");

            result = OK;
            goto out;
        }
    }

    found = FindVar((STRPTR) name, 0);
    if (found == NULL) {
        SHOWMSG("the local variable is not yet set; remembering that");

        lv = malloc(sizeof(*lv) + strlen(name) + 1);
        if (lv == NULL) {
            SHOWMSG("not enough memory to remember local variable to be deleted");
            goto out;
        }

        lv->lv_Next = __lv_root;
        __lv_root = lv;

        lv->lv_Name = (char *) (lv + 1);
        strcpy(lv->lv_Name, name);
    }

    if (value == NULL)
        value = "";

    SHOWSTRING(name);
    SHOWSTRING(value);

    status = SetVar((STRPTR) name, (STRPTR) value, (LONG) strlen(value), 0);
    if (status == DOSFALSE) {
        SHOWMSG("could not set variable");

        if (lv != NULL) {
            __lv_root = lv->lv_Next;
            free(lv);
        }

        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    result = OK;

out:

    if (name_copy != NULL)
        free(name_copy);

    RETURN(result);
    return (result);
}
