/*
 * $Id: profile_gmon.c,v 1.1 2023-10-20 12:04:26 clib4devs Exp $
*/

#include <exec/exec.h>
#include <dos/dos.h>
#include <libraries/elf.h>
#include <proto/dos.h>
#include <proto/elf.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <macros.h>
#include <sys/uio.h>

#define SCALE_1_TO_1 0x10000L
#define MIN_OS_VERSION 52

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "gmon.h"
#include "gmon_out.h"

/*  Head of basic-block list or NULL. */
struct __bb *__bb_head __attribute__ ((visibility ("hidden")));

struct gmonparam _gmonparam = {
        state : kGmonProfOn
};

static unsigned int s_scale;

void
write_hist(int fd) {
    u_char tag = GMON_TAG_TIME_HIST;

    if (_gmonparam.kcountsize > 0) {
        struct iovec iov[3] = {
            { &tag, sizeof(tag) },
            { &thdr, sizeof(struct gmon_hist_hdr) },
            { _gmonparam.kcount, _gmonparam.kcountsize }
        };

        if (
                sizeof(thdr) != sizeof(struct gmon_hist_hdr) ||
                (offsetof(struct real_gmon_hist_hdr, low_pc) != offsetof(struct gmon_hist_hdr, low_pc)) ||
                (offsetof(struct real_gmon_hist_hdr, high_pc) != offsetof(struct gmon_hist_hdr, high_pc)) ||
                (offsetof(struct real_gmon_hist_hdr, hist_size) != offsetof(struct gmon_hist_hdr, hist_size)) ||
                (offsetof(struct real_gmon_hist_hdr, prof_rate) != offsetof(struct gmon_hist_hdr, prof_rate)) ||
                (offsetof(struct real_gmon_hist_hdr, dimen) != offsetof(struct gmon_hist_hdr, dimen)) ||
                (offsetof(struct real_gmon_hist_hdr, dimen_abbrev) != offsetof(struct gmon_hist_hdr, dimen_abbrev))
        )
            return;

        thdr.low_pc =  (char *) _gmonparam.text_start;
        thdr.high_pc = (char *) _gmonparam.text_start + _gmonparam.highpc - _gmonparam.lowpc;
        thdr.hist_size = _gmonparam.kcountsize / sizeof(HISTCOUNTER);
        dprintf("thdr.low_pc = %x - thdr.high_pc = %x\n", thdr.low_pc, thdr.high_pc);
        thdr.prof_rate = 100;
        strncpy(thdr.dimen, "seconds", sizeof(thdr.dimen));
        thdr.dimen_abbrev = 's';

        writev(fd, iov, 3);
    }
}

void
write_call_graph(int fd) {
    u_char tag = GMON_TAG_CG_ARC;
    struct gmon_cg_arc_record raw_arc[NARCS_PER_WRITEV] __attribute__((aligned(__alignof__(char *))));
    ARCINDEX from_index, to_index;
    u_long from_len;
    u_long frompc;
    struct iovec iov[2 * NARCS_PER_WRITEV];
    int nfilled;

    for (nfilled = 0; nfilled < NARCS_PER_WRITEV; ++nfilled) {
        iov[2 * nfilled].iov_base = &tag;
        iov[2 * nfilled].iov_len = sizeof(tag);

        iov[2 * nfilled + 1].iov_base = &raw_arc[nfilled];
        iov[2 * nfilled + 1].iov_len = sizeof(struct gmon_cg_arc_record);
    }

    nfilled = 0;
    from_len = _gmonparam.fromssize / sizeof(*_gmonparam.froms);
    for (from_index = 0; from_index < from_len; ++from_index) {
        if (_gmonparam.froms[from_index] == 0)
            continue;

        frompc = _gmonparam.text_start;
        frompc += from_index * _gmonparam.hashfraction * sizeof(*_gmonparam.froms);
        for (to_index = _gmonparam.froms[from_index];
             to_index != 0;
             to_index = _gmonparam.tos[to_index].link) {
            struct arc {
                char *frompc;
                char *selfpc;
                int32_t count;
            } arc;

            arc.frompc = (char *) frompc;
            arc.selfpc = (char *) _gmonparam.text_start + _gmonparam.tos[to_index].selfpc;
            arc.count = _gmonparam.tos[to_index].count;
            dprintf("arc.frompc = %p - arc.selfpc = %p\n", arc.frompc, arc.selfpc);
            memcpy(raw_arc + nfilled, &arc, sizeof(raw_arc[0]));

            if (++nfilled == NARCS_PER_WRITEV) {
                writev(fd, iov, 2 * nfilled);
                nfilled = 0;
            }
        }
    }
    if (nfilled > 0)
        writev(fd, iov, 2 * nfilled);
}

void
write_bb_counts(int fd) {
    struct __bb *grp;
    u_char tag = GMON_TAG_BB_COUNT;
    size_t ncounts;
    size_t i;

    struct iovec bbhead[2] = {
    {&tag, sizeof(tag)},
    {&ncounts, sizeof(ncounts)}
    };
    struct iovec bbbody[8];
    size_t nfilled;

    for (i = 0; i < (sizeof(bbbody) / sizeof(bbbody[0])); i += 2) {
        bbbody[i].iov_len = sizeof(grp->addresses[0]);
        bbbody[i + 1].iov_len = sizeof(grp->counts[0]);
    }

    /* Write each group of basic-block info (all basic-blocks in a
       compilation unit form a single group). */

    for (grp = __bb_head; grp; grp = grp->next) {
        ncounts = grp->ncounts;
        writev(fd, bbhead, 2);
        for (nfilled = i = 0; i < ncounts; ++i) {
            if (nfilled > (sizeof(bbbody) / sizeof(bbbody[0])) - 2) {
                writev(fd, bbbody, nfilled);
                nfilled = 0;
            }

            bbbody[nfilled++].iov_base = (char *)&grp->addresses[i];
            bbbody[nfilled++].iov_base = &grp->counts[i];
        }
        if (nfilled > 0)
            writev(fd, bbbody, nfilled);
    }
}

void monstartup(uint32 low_pc, uint32 high_pc) {
    uint8 *cp;
    uint32 lowpc, highpc;
    uint32 text_start = 0;
    struct gmonparam *p = &_gmonparam;
    int o;

    dprintf("in monstartup\n");

    /*
     * If we don't get proper lowpc and highpc, then
     * we'll try to get them from the elf handle.
     */
    if (low_pc == 0 && high_pc == 0) {
        mongetpcs(&lowpc, &highpc, &text_start);
    } else {
        p->text_start = 0x01000074; // Default to our default text segment start
        lowpc = low_pc;
        highpc = high_pc;
    }
    p->text_start = text_start;

    /*
     * Round lowpc and highpc to multiples of the density
     * to prevent using floating point scaling
     */
    p->lowpc = ROUNDDOWN(lowpc, HISTFRACTION * sizeof(HISTCOUNTER));
    p->highpc = ROUNDUP(highpc, HISTFRACTION * sizeof(HISTCOUNTER));

    /* Size of the text segment */
    p->textsize = p->highpc - p->lowpc;

    /*
     * Size of the histogram. Due to the nature of PowerPC code,
     * we can safely use a histogram fraction of at least 4, since
     * every instruction is exactly one word wide and always aligned.
     */
    p->kcountsize = p->textsize / HISTFRACTION;
    p->log_hashfraction = -1;

    /*
     * The hash table size
     */
    p->hashfraction = HASHFRACTION;

    /* The following test must be kept in sync with the corresponding test in _mcount.c.  */
    if ((HASHFRACTION & (HASHFRACTION - 1)) == 0) {
        /* if HASHFRACTION is a power of two, mcount can use shifting
       instead of integer division.  Precompute shift amount. */
        p->log_hashfraction = ffs(p->hashfraction * sizeof(*p->froms)) - 1;
    }
    p->fromssize = p->textsize / HASHFRACTION;

    p->tolimit = p->textsize * ARCDENSITY / 100;
    if (p->tolimit < MINARCS)
        p->tolimit = MINARCS;
    else if (p->tolimit > MAXARCS)
        p->tolimit = MAXARCS;

    p->tossize = p->tolimit * sizeof(struct tostruct);

    dprintf("lowpc = %p\n", lowpc);
    dprintf("highpc = %p\n", highpc);
    dprintf("text_start = %p\n", p->text_start);
    dprintf("textsize = %d\n", p->textsize);
    dprintf("kcountsize = %d\n", p->kcountsize);
    dprintf("fromssize = %d\n", p->fromssize);
    dprintf("tolimit = %d, tossize = %d\n", p->tolimit, p->tossize);

    cp = (uint8 *) calloc(1, p->kcountsize + p->fromssize + p->tossize);
    if (!cp) {
        p->state = kGmonProfError;
        return;
    }

    p->tos = (struct tostruct *) cp;
    cp += p->tossize;

    p->kcount = (HISTCOUNTER *) cp;
    cp += p->kcountsize;

    p->froms = (ARCINDEX *) cp;

    p->tos[0].link = 0;

    /* Verify granularity for sampling */
    o = p->highpc - p->lowpc;
    if (p->kcountsize < (u_long) o) {
        /* avoid floating point operations */
        int quot = o / p->kcountsize;

        if (quot >= 0x10000)
            s_scale = 1;
        else if (quot >= 0x100)
            s_scale = 0x10000 / quot;
        else if (o >= 0x800000)
            s_scale = 0x1000000 / (o / (p->kcountsize >> 8));
        else
            s_scale = 0x1000000 / ((o << 8) / p->kcountsize);
    }
    else
        s_scale = SCALE_1_TO_1;

    s_scale >>= 1;
    dprintf("Enabling monitor: Scale = %d\n", s_scale);
    moncontrol(1);
}

void moncontrol(int mode) {
    struct gmonparam *p = &_gmonparam;

    /* Don't change the state if we ran into an error.  */
    if (p->state == kGmonProfError)
        return;

    if (mode) {
        /* Start profiling. */
        profil((uint16 *) p->kcount, (size_t) p->kcountsize, p->lowpc, s_scale);
        p->state = kGmonProfOn;
    } else {
        /* Stop Profiling. */
        profil(NULL, 0, (uint32) 0, 0);
        p->state = kGmonProfOff;
    }
}

void moncleanup(void) {
    int fd;
    struct gmonparam *p = &_gmonparam;

    moncontrol(0);

    if (p->state == kGmonProfError) {
        fprintf(stderr, "WARNING: Overflow during profiling\n");
        goto out;
    }

    if (_gmonparam.kcountsize > 0) {
        fd = open("gmon.out", O_CREAT | O_TRUNC | O_WRONLY);
        if (!fd) {
            fprintf(stderr, "ERROR: could not open gmon.out\n");
            goto out;
        }

        /* write gmon.out header: */
        struct real_gmon_hdr {
            char cookie[4];
            int32_t version;
            char spare[3 * 4];
        } ghdr;

        if (
            sizeof(ghdr) != sizeof(struct gmon_hdr) ||
            (offsetof(struct real_gmon_hdr, cookie) != offsetof(struct gmon_hdr, cookie)) ||
            (offsetof(struct real_gmon_hdr, version) != offsetof(struct gmon_hdr, version))) {
                goto out;
        }

        memcpy(&ghdr.cookie[0], GMON_MAGIC, sizeof(ghdr.cookie));
        ghdr.version = GMON_VERSION;
        memset(ghdr.spare, '\0', sizeof(ghdr.spare));
        write(fd, &ghdr, sizeof(struct gmon_hdr));

        /* write PC histogram: */
        write_hist(fd);

        /* write call-graph: */
        write_call_graph(fd);

        /* write basic-block execution counts: */
        write_bb_counts(fd);

        close(fd);
    }
out:
    if (p->tos) {
        free(p->tos);
        p->tos = NULL;
    }

}

void mongetpcs(uint32 *lowpc, uint32 *highpc, uint32 *text_start) {
    struct Library *ElfBase = NULL;
    struct ElfIFace *IElf = NULL;
    struct Process *self;
    BPTR seglist;
    Elf32_Handle elfHandle;
    uint32 i;
    Elf32_Shdr *shdr;
    uint32 numSections;

    *lowpc = 0;
    *highpc = 0;

    ElfBase = OpenLibrary("elf.library", 0L);
    if (!ElfBase)
        goto out;

    IElf = (struct ElfIFace *) GetInterface(ElfBase, "main", 1, NULL);
    if (!IElf)
        goto out;

    self = (struct Process *) FindTask(0);
    seglist = GetProcSegList(self, GPSLF_CLI | GPSLF_SEG);

    GetSegListInfoTags(seglist, GSLI_ElfHandle, &elfHandle, TAG_DONE);
    elfHandle = OpenElfTags(OET_ElfHandle, elfHandle, OET_ReadOnlyCopy, TRUE, TAG_DONE);

    if (!elfHandle)
        goto out;

    GetElfAttrsTags(elfHandle, EAT_NumSections, &numSections, TAG_DONE);

    for (i = 0; i < numSections; i++) {
        shdr = GetSectionHeaderTags(elfHandle, GST_SectionIndex, i, TAG_DONE);
        if (shdr && (shdr->sh_flags & SWF_EXECINSTR)) {
            uint32 base = (uint32) GetSectionTags(elfHandle, GST_SectionIndex, i, TAG_DONE);
            *lowpc = base;
            *highpc = base + shdr->sh_size;
            *text_start = shdr->sh_addr;
            break;
        }
    }

    CloseElfTags(elfHandle, CET_ReClose, TRUE, TAG_DONE);

out:
    if (IElf)
        DropInterface((struct Interface *) IElf);
    if (ElfBase)
        CloseLibrary(ElfBase);
}

#include "macros.h"

int __profiler_init(void) __attribute__((constructor));
void __profiler_exit(void) __attribute__((destructor));

int __profiler_init(void) {
    monstartup(0, 0);
    return OK;
}

void __profiler_exit(void) {
    moncleanup();
}
