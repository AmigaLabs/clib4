/*
 * $Id: timezone_headers.h,v 1.0 2020-01-15 10:00:23 clib4devs Exp $
*/

#ifndef _TIMEZONE_HEADERS_H
#define _TIMEZONE_HEADERS_H

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif /* EXEC_LIBRARIES_H */

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif /* PROTO_EXEC_H */

#include <locale.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>


#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#ifndef _DEBUG_H
#include "debug.h"
#endif /* _DEBUG_H */

#include <time.h>

extern void __timezone_lock(void);
extern void __timezone_unlock(void);

struct os_tzrule_t
{
    /*
     *   Day of year, 1-365, NEVER counting Feb 29; set to 0 if not used.
     *   Corresponds to the "J" format in Unix TZ strings.  (Called "Julian
     *   day" in the POSIX docs, thus the "J", even though it's a bit of a
     *   misnomer.)(Because of the invariance of the mapping from J-number to
     *   date, this is just an obtuse way of specifying a month/day date.
     *   But even so, we'll let the OS layer relay this back to us in
     *   J-number format and count on the portable caller to work out the
     *   date, rather than foisting that work on each platform
     *   implementation.)
     */
    int jday;

    /*
     *   Day of year, 1-366, counting Feb 29 on leap years; set to 0 if not
     *   used; ignored if 'jday' is nonzero.  This corresponds to the Julian
     *   day sans "J" in TZ strings (almost - that TZ format uses 0-365 as
     *   its range, so bump it up by one when parsing a TZ string).  This
     *   format is even more obtuse than the J-day format, in that it doesn't
     *   even have an invariant month/day mapping (not after day 59, anyway -
     *   day 60 is either February 29 or March 1, depending on the leapness
     *   of the year, and every day after that is similarly conditional).  As
     *   far as I can tell, no one uses this option, so I'm not sure why it
     *   exists.  The zoneinfo source format doesn't have a way to represent
     *   it, which says to me that no one has ever used it in a statutory DST
     *   start/end date definition in the whole history of time zones around
     *   the world, since the whole history of time zones around the world is
     *   exactly what the zoneinfo database captures in exhaustive and
     *   painstaking detail.  If anyone had ever used it in defining a time
     *   zone, zoneinfo would have an option for it.  My guess is that it's a
     *   fossilized bug from some early C RTL that's been retained out of an
     *   abundance of caution vis-a-vis compatibility, and was entirely
     *   replaced in practice by the J-number format as soon as someone
     *   noticed the fiddly leap year behavior.  But for the sake of
     *   completeness...
     */
    int yday;

    /*
     *   The month (1-12), week of the month, and day of the week (1-7 for
     *   Sunday to Saturday).  Week 1 is the first week in which 'day'
     *   occurs, week 2 is the second, etc.; week 5 is the last occurrence of
     *   'day' in the month.  These fields are used for "second Sunday in
     *   March" types of rules.  Set these to zero if they're not used;
     *   they're ignored in any case if 'jday' or 'yday' are non-zero.
     */
    int month;
    int week;
    int day;

    /* time of day, in seconds after midnight (e.g., 2AM is 120 == 2*60*60) */
    int time;
};

struct os_tzinfo_t
{
    /*
     *   The local offset from GMT, in seconds, for standard time and
     *   daylight time in this zone.  These values are positive for zones
     *   east of GMT and negative for zones west: New York standard time
     *   (EST) is 5 hours west of GMT, so its offset is -5*60*60.
     *
     *   Set both of these fields (if possible) regardless of whether
     *   standard or daylight time is currently in effect in the zone.  The
     *   caller will select which offset to use based on the start/end rules,
     *   or based on the 'is_dst' flag if no rules are available.
     *
     *   If it's only possible to determine the current wall clock offset, be
     *   it standard or daylight time, and it's not possible to determine the
     *   time difference between the two, simply set both of these to the
     *   current offset.  This information isn't available from the standard
     *   C library, and many OS APIs also lack it.
     */
    int32_t std_ofs;
    int32_t dst_ofs;

    /*
     *   The abbreviations for the local zone's standard time and daylight
     *   time, respectively, when displaying date/time values.  E.g., "EST"
     *   and "EDT" for US Eastern Time.  If the zone doesn't observe daylight
     *   time (it's on standard time year round), set dst_abbr to an empty
     *   string.
     *
     *   As with std_ofs and dst_ofs, you can set both of these to the same
     *   string if it's only possible to determine the one that's currently
     *   in effect.
     */
    char std_abbr[16];
    char dst_abbr[16];

    /*
     *   The ongoing rules for switching between daylight and standard time
     *   in this zone, if available.  'dst_start' is the date when daylight
     *   savings starts, 'dst_end' is the date when standard time resumes.
     *   Set all fields to 0 if the start/stop dates aren't available, or the
     *   zone is on standard time year round.
     */
    struct os_tzrule_t dst_start;
    struct os_tzrule_t dst_end;

    /*
     *   True -> the zone is CURRENTLY on daylight savings time; false means
     *   it's currently on standard time.
     *
     *   This is only used if the start/end rules aren't specified.  In the
     *   absence of start/end rules, there's no way to know when the current
     *   standard/daylight phase ends, so we'll have to assume that the
     *   current mode is in effect permanently.  In this case, the caller
     *   will use only be able to use the offset and abbreviation for the
     *   current mode and will have to ignore the other one.
     */
    int is_dst;
};
int oss_parse_posix_tz(struct os_tzinfo_t *info, const char *tz, size_t len, int west_is_positive);

void _tzset_unlocked(void);
int __tzcalc_limits(int year);

#define SECSPERMIN	60L
#define MINSPERHOUR	60L
#define HOURSPERDAY	24L
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	(SECSPERHOUR * HOURSPERDAY)
#define DAYSPERWEEK	7
#define MONSPERYEAR	12

#define YEAR_BASE	1900
#define EPOCH_YEAR      1970
#define EPOCH_WDAY      4
#define EPOCH_YEARS_SINCE_LEAP 2
#define EPOCH_YEARS_SINCE_CENTURY 70
#define EPOCH_YEARS_SINCE_LEAP_CENTURY 370

extern const uint8_t __month_lengths[2][MONSPERYEAR];

static inline int isleap (int y) {
    // This routine must return exactly 0 or 1, because the result is used to index on __month_lengths[].
    // The order of checks below is the fastest for a random year.
    return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
}


#endif /* _TIMEZONE_HEADERS_H */
