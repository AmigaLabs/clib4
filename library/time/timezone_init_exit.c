/*
 * $Id: timezone_init_exit.c,v 1.2 2024-04-09 10:01:23 clib4devs Exp $
*/

#ifndef _TIMEZONE_HEADERS_H
#include "timezone_headers.h"
#endif /* _TIMEZONE_HEADERS_H */

void
__timezone_exit(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    __timezone_lock();

    if (__clib4->__TimezoneBase != NULL) {
        DECLARE_TIMEZONEBASE();

        if (__clib4->__dyntz == TRUE) {
            if (__clib4->__tzname[0]) FreeVec(__clib4->__tzname[0]);
            if (__clib4->__tzname[1]) FreeVec(__clib4->__tzname[1]);
            __clib4->__tzname[0] = NULL;
            __clib4->__tzname[1] = NULL;
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
        __clib4->__TimezoneBase = OpenLibrary("timezone.library", 52);

        if (__clib4->__TimezoneBase != NULL) {
            __clib4->__ITimezone = (struct TimezoneIFace *) GetInterface(__clib4->__TimezoneBase, "main", 1, 0);
            if (__clib4->__ITimezone == NULL) {
                CloseLibrary(__clib4->__TimezoneBase);
                __clib4->__TimezoneBase = NULL;
            }
        }
    }

    if (__clib4->__TimezoneBase != NULL) {
        DECLARE_TIMEZONEBASE();

        // Set global timezone variable
        uint32 gmtoffset = 0;
        int8 dstime = -1;
        __clib4->__tzname[0] = AllocVecTags(MAX_TZSIZE + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
        __clib4->__tzname[1] = AllocVecTags(MAX_TZSIZE + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
        __clib4->__dyntz = TRUE;

        GetTimezoneAttrs(NULL,
                         TZA_Timezone, __clib4->__tzname[0],
                         TZA_TimezoneSTD, __clib4->__tzname[1],
                         TZA_UTCOffset, &gmtoffset,
                         TZA_TimeFlag, &dstime,
                         TAG_DONE);

        __clib4->__timezone = 60 * gmtoffset;
        __clib4->__daylight = dstime & TFLG_ISDST;

        result = OK;
    } else {
        /* default values */
        __clib4->__timezone = 0;
        __clib4->__daylight = 0;
        __clib4->__tzname[0] = (char *) "GMT";
        __clib4->__tzname[1] = (char *) "AMT";
        __clib4->__dyntz = FALSE;
    }

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

void _timezone_exit(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    __timezone_exit();

    __delete_semaphore(__clib4->timezone_lock);
    __clib4->timezone_lock = NULL;

    LEAVE();
}

void _timezone_init(void) {
    ENTER();

    struct _clib4 *__clib4 = __CLIB4;

    __clib4->timezone_lock = __create_semaphore();
    if (__clib4->timezone_lock == NULL)
        goto out;

    __timezone_init();

out:

    LEAVE();
}
