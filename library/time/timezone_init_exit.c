/*
 * $Id: timezone_init_exit.c,v 1.0 2021-01-15 10:01:23 clib2devs Exp $
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
    struct _clib2 *__clib2 = __CLIB2;

    __timezone_lock();

    if (__clib2->__TimezoneBase != NULL) {
        DECLARE_TIMEZONEBASE();

        if (__clib2->__dyntz == TRUE) {
            if (__clib2->__tzname[0]) FreeVec(__clib2->__tzname[0]);
            if (__clib2->__tzname[1]) FreeVec(__clib2->__tzname[1]);
            __clib2->__tzname[0] = NULL;
            __clib2->__tzname[1] = NULL;
        }

        if (__clib2->__ITimezone != NULL) {
            DropInterface((struct Interface *) __clib2->__ITimezone);
            __clib2->__ITimezone = NULL;
        }

        CloseLibrary(__clib2->__TimezoneBase);
        __clib2->__TimezoneBase = NULL;
    }

    __timezone_unlock();

    LEAVE();
}

int
__timezone_init(void) {
    ENTER();

    int result = ERROR;
    struct _clib2 *__clib2 = __CLIB2;

    __timezone_lock();

    if (__clib2->__TimezoneBase == NULL) {
        __clib2->__TimezoneBase = OpenLibrary("timezone.library", 52);

        if (__clib2->__TimezoneBase != NULL) {
            __clib2->__ITimezone = (struct TimezoneIFace *) GetInterface(__clib2->__TimezoneBase, "main", 1, 0);
            if (__clib2->__ITimezone == NULL) {
                CloseLibrary(__clib2->__TimezoneBase);
                __clib2->__TimezoneBase = NULL;
            }
        }
    }

    if (__clib2->__TimezoneBase != NULL) {
        DECLARE_TIMEZONEBASE();

        // Set global timezone variable
        uint32 gmtoffset = 0;
        int8 dstime = -1;
        __clib2->__tzname[0] = AllocVecTags(MAX_TZSIZE + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
        __clib2->__tzname[1] = AllocVecTags(MAX_TZSIZE + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
        __clib2->__dyntz = TRUE;

        GetTimezoneAttrs(NULL,
                         TZA_Timezone, __clib2->__tzname[0],
                         TZA_TimezoneSTD, __clib2->__tzname[1],
                         TZA_UTCOffset, &gmtoffset,
                         TZA_TimeFlag, &dstime,
                         TAG_DONE);

        __clib2->__timezone = 60 * gmtoffset;
        __clib2->__daylight = dstime & TFLG_ISDST;

        result = OK;
    } else {
        /* default values */
        __clib2->__timezone = 0;
        __clib2->__daylight = 0;
        __clib2->__tzname[0] = (char *) "GMT";
        __clib2->__tzname[1] = (char *) "AMT";
        __clib2->__dyntz = FALSE;
    }

    __timezone_unlock();

    RETURN(result);
    return (result);
}

void
__timezone_lock(void) {
    struct _clib2 *__clib2 = __CLIB2;

    if (__clib2->timezone_lock != NULL)
        ObtainSemaphore(__clib2->timezone_lock);
}

void
__timezone_unlock(void) {
    struct _clib2 *__clib2 = __CLIB2;

    if (__clib2->timezone_lock != NULL)
        ReleaseSemaphore(__clib2->timezone_lock);
}

CLIB_DESTRUCTOR(timezone_exit) {
    ENTER();
    struct _clib2 *__clib2 = __CLIB2;

    __timezone_exit();

    __delete_semaphore(__clib2->timezone_lock);
    __clib2->timezone_lock = NULL;

    LEAVE();
}

CLIB_CONSTRUCTOR(timezone_init) {
    ENTER();

    BOOL success = FALSE;
    struct _clib2 *__clib2 = __CLIB2;

    __clib2->timezone_lock = __create_semaphore();
    if (__clib2->timezone_lock == NULL)
        goto out;

    __timezone_init();

    success = TRUE;

out:

    SHOWVALUE(success);
    LEAVE();

    if (success)
        CONSTRUCTOR_SUCCEED();
    else
        CONSTRUCTOR_FAIL();
}
