/*
 * $Id: timezone_init_exit.c,v 1.0 2021-01-15 10:01:23 clib4devs Exp $
*/

#ifndef _TIMEZONE_HEADERS_H
#include "timezone_headers.h"
#endif /* _TIMEZONE_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

void
__timezone_exit(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    __timezone_lock();

    if (__clib4->__TimezoneBase != NULL) {
        DECLARE_TIMEZONEBASE_R(__clib4);

        if (__clib4->__dyntz == TRUE) {
            if (__clib4->__tzname[0]) free(__clib4->__tzname[0]);
            if (__clib4->__tzname[1]) free(__clib4->__tzname[1]);
            __clib4->__tzname[0] = NULL;
            __clib4->__tzname[1] = NULL;
            __clib4->__dyntz = FALSE;
        }

        if (__clib4->__ITimezone != NULL) {
            DropInterface((struct Interface *) __clib4->__ITimezone);
            __clib4->__ITimezone = NULL;
        }

        CloseLibrary(__clib4->__TimezoneBase);
        __clib4->__TimezoneBase = NULL;
    }

    __timezone_unlock();

    LEAVE();
}

int
__timezone_init(void) {
    ENTER();

    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    __timezone_lock();

    if (__clib4->__TimezoneBase == NULL) {
        SHOWMSG("Opening timezone.library");
        __clib4->__TimezoneBase = OpenLibrary("timezone.library", 52);

        if (__clib4->__TimezoneBase != NULL) {
            __clib4->__ITimezone = (struct TimezoneIFace *) GetInterface(__clib4->__TimezoneBase, "main", 1, 0);
            if (__clib4->__ITimezone == NULL) {
                CloseLibrary(__clib4->__TimezoneBase);
                __clib4->__TimezoneBase = NULL;
                SHOWMSG("Cannot get timezone interface");
                goto out;
            }
        }
        else {
            SHOWMSG("Fail to open timezone.library");
            goto out;
        }
    }

    if (__clib4->__TimezoneBase != NULL) {
        DECLARE_TIMEZONEBASE_R(__clib4);

        // Set global timezone variable
        uint32 gmtoffset = 0;
        int8 dstime = -1;
        __clib4->__tzname[0] = malloc(MAX_TZSIZE + 1);
        __clib4->__tzname[1] = malloc(MAX_TZSIZE + 1);
        __clib4->__dyntz = TRUE;

        GetTimezoneAttrs(NULL,
                         TZA_Timezone, __clib4->__tzname[0],
                         TZA_TimezoneSTD, __clib4->__tzname[1],
                         TZA_UTCOffset, &gmtoffset,
                         TZA_TimeFlag, &dstime,
                         TAG_DONE);

        __clib4->__timezone = 60 * gmtoffset;
        __clib4->__daylight = dstime & TFLG_ISDST;
    } else {
        /* default values */
        __clib4->__timezone = 0;
        __clib4->__daylight = 0;
        __clib4->__tzname[0] = (char *) "GMT";
        __clib4->__tzname[1] = (char *) "AMT";
        __clib4->__dyntz = FALSE;

    }

    result = OK;
out:
    __timezone_unlock();

    RETURN(result);
    return (result);
}

void
__timezone_lock(void) {
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->timezone_lock != NULL)
        ObtainSemaphore(__clib4->timezone_lock);
}

void
__timezone_unlock(void) {
    struct _clib4 *__clib4 = __CLIB4;

    if (__clib4->timezone_lock != NULL)
        ReleaseSemaphore(__clib4->timezone_lock);
}

CLIB_DESTRUCTOR(timezone_exit) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    __timezone_exit();

    __delete_semaphore(__clib4->timezone_lock);
    __clib4->timezone_lock = NULL;

    LEAVE();
}

CLIB_CONSTRUCTOR(timezone_init) {
    ENTER();

    BOOL success = FALSE;
    struct _clib4 *__clib4 = __CLIB4;

    __clib4->timezone_lock = __create_semaphore();
    if (__clib4->timezone_lock == NULL)
        goto out;

    success = (__timezone_init() == OK);

out:

    SHOWVALUE(success);
    LEAVE();

    if (success)
        CONSTRUCTOR_SUCCEED();
    else
        CONSTRUCTOR_FAIL();
}
