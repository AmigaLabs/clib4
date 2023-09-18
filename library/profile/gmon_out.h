/*
* $Id: profile_gmon_out.h,v 1.0 2023-09-17 12:04:26 clib2devs Exp $
*/

/* This file specifies the format of gmon.out files.  It should have
   as few external dependencies as possible as it is going to be included
   in many different programs.  That is, minimize the number of #include's.

   A gmon.out file consists of a header (defined by gmon_hdr) followed by
   a sequence of records.  Each record starts with a one-byte tag
   identifying the type of records, followed by records specific data. */

#ifndef _SYS_GMON_OUT_H
#define _SYS_GMON_OUT_H    1

#include <features.h>

#define GMON_MAGIC      "gmon"    /* magic cookie */
#define GMON_VERSION    1         /* version number */
//#define GMON_VERSION     0x00051879

/* For profiling shared object we need a new format.  */
#define GMON_SHOBJ_VERSION  0x1ffff
#define NARCS_PER_WRITEV    32

__BEGIN_DECLS

/*
 * Raw header as it appears on file (without padding).  This header
 * always comes first in gmon.out and is then followed by a series
 * records defined below.
 */
struct gmon_hdr {
    char cookie[4];
    char version[4];
    char spare[3 * 4];
};

/* types of records in this file: */
typedef enum {
    GMON_TAG_TIME_HIST = 0,
    GMON_TAG_CG_ARC = 1,
    GMON_TAG_BB_COUNT = 2
} GMON_Record_Tag;

struct gmon_hist_hdr {
    char low_pc[sizeof(char *)];    /* base pc address of sample buffer */
    char high_pc[sizeof(char *)];    /* max pc address of sampled buffer */
    char hist_size[4];            /* size of sample buffer */
    char prof_rate[4];            /* profiling clock rate */
    char dimen[15];            /* phys. dim., usually "seconds" */
    char dimen_abbrev;            /* usually 's' for "seconds" */
};

struct gmon_cg_arc_record {
    char from_pc[sizeof(char *)];    /* address within caller's body */
    char self_pc[sizeof(char *)];    /* address within callee's body */
    char count[4];            /* number of arc traversals */
};

struct real_gmon_hist_hdr {
    char *low_pc;
    char *high_pc;
    int32_t hist_size;
    int32_t prof_rate;
    char dimen[15];
    char dimen_abbrev;
} thdr;

void write_hist(int fd);
void write_call_graph(int fd);
void write_bb_counts(int fd);

__END_DECLS

#endif /* sys/gmon_out.h */
