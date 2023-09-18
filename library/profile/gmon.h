/*
* $Id: profile_gmon.h,v 1.0 2021-01-18 12:04:26 clib2devs Exp $
*/

#ifndef _GMON_H
#define _GMON_H

#include <exec/types.h>
#include "../shared_library/math.h"

#undef DebugPrintF
#define dprintf(format, args...) ((struct ExecIFace *)((*(struct ExecBase **)4)->MainInterface))->DebugPrintF("[%s] " format, __PRETTY_FUNCTION__, ##args)

void moncontrol(int);
void monstartup(uint32, uint32);
void moncleanup(void);
void mongetpcs(uint32 *lowpc, uint32 *highpc);

struct gmonhdr {
    uint32 lpc;
    uint32 hpc;
    int ncnt;
    int version;
    int profrate;
    int reserved[3];
};

#define HISTCOUNTER uint16

// I am sure we can make these bigger
#define HISTFRACTION 2
#define HASHFRACTION 2

/*
 * Percent of text space to allocate for tostructs.
 * This is a heuristic; we will fail with a warning when profiling programs
 * with a very large number of very small functions, but that's
 * normally OK.
 * 2 is probably still a good value for normal programs.
 * Profiling a test case with 64000 small functions will work if
 * you raise this value to 3 and link statically (which bloats the
 * text size, thus raising the number of arcs expected by the heuristic).
 */
#define ARCDENSITY	3

/*
 * Always allocate at least this many tostructs.  This
 * hides the inadequacy of the ARCDENSITY heuristic, at least
 * for small programs.
 */
#define MINARCS      50

/*
 * Maximum number of arcs we want to allow.
 * Used to be max representable value of ARCINDEX minus 2, but now
 * that ARCINDEX is a long, that's too large; we don't really want
 * to allow a 48 gigabyte table.
 * The old value of 1<<16 wasn't high enough in practice for large C++
 * programs; will 1<<20 be adequate for long?  FIXME
 */
#define MAXARCS (1 << 20)

/*
 * The type used to represent indices into gmonparam.tos[].
 */
#define	ARCINDEX uint32

/* structure emitted by "gcc -a".  This must match struct bb in
   gcc/libgcc2.c.  It is OK for gcc to declare a longer structure as
   long as the members below are present.  */
struct __bb {
    long			     zero_word;
    const char		    *filename;
    long			    *counts;
    long			     ncounts;
    struct __bb		    *next;
    const unsigned long	*addresses;
};

extern struct __bb *__bb_head;

struct tostruct {
    uint32   selfpc;
    int32    count;
    ARCINDEX link;
};

struct rawarc {
    uint32  raw_frompc;
    uint32  raw_selfpc;
    int32   raw_count;
};

#define ROUNDDOWN(x, y) (((x)/(y))*(y))
#define ROUNDUP(x, y)   ((((x)+(y)-1)/(y))*(y))

struct gmonparam {
    int state;
    uint16          *kcount;
    uint32           kcountsize;
    ARCINDEX        *froms;
    uint32           fromssize;
    struct tostruct *tos;
    uint32           tossize;
    int32            tolimit;
    uint32           lowpc;
    uint32           highpc;
    uint32           textsize;
    uint32           hashfraction;
    long             log_hashfraction;
};

extern struct gmonparam _gmonparam;

enum {
    kGmonProfOn = 0,
    kGmonProfBusy = 1,
    kGmonProfError = 2,
    kGmonProfOff = 3
};

enum {
    kGprofState = 0,
    kGprofCount = 1,
    kGprofFroms = 2,
    kGprofTos = 3,
    kGprofGmonParam = 4
};

#endif
