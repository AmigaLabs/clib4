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

    __timezone_lock();

    if (__CLIB2->__TimezoneBase != NULL) {
        DECLARE_TIMEZONEBASE();

        if (__CLIB2->dyntz == TRUE) {
            if (__CLIB2->tzname[0]) FreeVec(__CLIB2->tzname[0]);
            if (__CLIB2->tzname[1]) FreeVec(__CLIB2->tzname[1]);
            __CLIB2->tzname[0] = NULL;
            __CLIB2->tzname[1] = NULL;
        }

        if (__CLIB2->__ITimezone != NULL) {
            DropInterface((struct Interface *) __CLIB2->__ITimezone);
            __CLIB2->__ITimezone = NULL;
        }

        CloseLibrary(__CLIB2->__TimezoneBase);
        __CLIB2->__TimezoneBase = NULL;
    }

    __timezone_unlock();

    LEAVE();
}

int
__timezone_init(void) {
    int result = ERROR;

    ENTER();

    __timezone_lock();

    if (__CLIB2->__TimezoneBase == NULL) {
        __CLIB2->__TimezoneBase = OpenLibrary("timezone.library", 52);

        if (__CLIB2->__TimezoneBase != NULL) {
            __CLIB2->__ITimezone = (struct TimezoneIFace *) GetInterface(__CLIB2->__TimezoneBase, "main", 1, 0);
            if (__CLIB2->__ITimezone == NULL) {
                CloseLibrary(__CLIB2->__TimezoneBase);
                __CLIB2->__TimezoneBase = NULL;
            }
        }
    }

    if (__CLIB2->__TimezoneBase != NULL) {
        DECLARE_TIMEZONEBASE();

        // Set global timezone variable
        uint32 gmtoffset = 0;
        int8 dstime = -1;
        __CLIB2->tzname[0] = AllocVecTags(MAX_TZSIZE + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
        __CLIB2->tzname[1] = AllocVecTags(MAX_TZSIZE + 1, AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_END);
        __CLIB2->dyntz = TRUE;

        GetTimezoneAttrs(NULL,
                         TZA_Timezone, __CLIB2->tzname[0],
                         TZA_TimezoneSTD, __CLIB2->tzname[1],
                         TZA_UTCOffset, &gmtoffset,
                         TZA_TimeFlag, &dstime,
                         TAG_DONE);

        __CLIB2->timezone = 60 * gmtoffset;
        __CLIB2->daylight = dstime & TFLG_ISDST;

        result = OK;
    } else {
        /* default values */
        __CLIB2->timezone = 0;
        __CLIB2->daylight = 0;
        __CLIB2->tzname[0] = (char *) "GMT";
        __CLIB2->tzname[1] = (char *) "AMT";
        __CLIB2->dyntz = FALSE;
    }

    __timezone_unlock();

    RETURN(result);
    return (result);
}

static struct SignalSemaphore *timezone_lock;

void
__timezone_lock(void) {
    if (timezone_lock != NULL)
        ObtainSemaphore(timezone_lock);
}

void
__timezone_unlock(void) {
    if (timezone_lock != NULL)
        ReleaseSemaphore(timezone_lock);
}

CLIB_DESTRUCTOR(timezone_exit) {
    ENTER();

    __timezone_exit();

    __delete_semaphore(timezone_lock);
    timezone_lock = NULL;

    LEAVE();
}

CLIB_CONSTRUCTOR(timezone_init) {
    BOOL success = FALSE;

    ENTER();

    timezone_lock = __create_semaphore();
    if (timezone_lock == NULL)
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
