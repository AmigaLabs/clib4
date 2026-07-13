/*
 * memcpy_guard.c -- expose tail over-read / over-write in clib4 memcpy/memmove.
 *
 * Why the existing test does not catch it:
 *   test_programs/memory/memcpy is a *benchmark*.  Its buffers carry 256 bytes
 *   of slack after the source (std::vector<char> src(size + 256)) and it never
 *   verifies the copied result -- so an implementation that touches a few bytes
 *   past the end of the source (or destination) is completely invisible to it.
 *
 * The bug this reproduces:
 *   clib4's AltiVec memcpy (library/cpu/altivec/vec_memcpy.sx, selected at run
 *   time on AltiVec CPUs by clib4.c:487 `IClib4->bcopy = vec_bcopy`) uses the
 *   classic "load two vectors + vperm" unaligned idiom, which can READ up to 15
 *   bytes past the end of the source.  Harmless in the middle of a heap, but it
 *   FAULTS (DSI) when the source ends just before an unmapped page -- which is
 *   what happens copying class/string data inside a VM.  It crashed JamVM on
 *   the QEMU 'amigaone' (G4) target: DSI at an lvx in clib4.library, DAR on a
 *   page boundary, while loading java2d class bytes.
 *
 * How this test forces the failure:
 *   It mmaps [data page][guard page] with the guard at PROT_NONE, then places
 *   the SOURCE so it ends EXACTLY at the guard boundary (phase A) and the
 *   DESTINATION so it ends exactly at the guard boundary (phase B).  Any read or
 *   write one byte past the requested length hits the guard and faults.  A
 *   SIGSEGV handler + siglongjmp lets the sweep continue and report every
 *   offending length; it also (for the first time) checks that the copy is
 *   actually correct.
 *
 *   If clib4 does NOT deliver SIGSEGV for the fault, the process hard-crashes
 *   (Grim Reaper) at the first offending case instead of reporting -- which is
 *   itself the proof.  The periodic "phase/len" breadcrumb on stderr narrows
 *   down which case did it.
 *
 * Scope: only exercises the AltiVec path on AltiVec CPUs (G4 and the QEMU
 *   'amigaone' machine).  On non-AltiVec CPUs (G3, 440/460, X5000/P5020,
 *   A1222/P1022) clib4 uses scalar mem ops and this test should PASS.
 *
 * Build: drop in test_programs/memory/; `make compile-tests` builds it
 *        (ppc-amigaos-gcc -mcrt=clib4 -fno-builtin ...).  PASS == rc 0.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/mman.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

static sigjmp_buf g_jmp;
static volatile sig_atomic_t g_faulted;

static void on_segv(int sig) {
    (void) sig;
    g_faulted = 1;
    siglongjmp(g_jmp, 1);       /* unwind back to the sigsetjmp in the sweep */
}

/* Map [data page][guard page]; guard is PROT_NONE so any access faults.
   Returns the data page base (NULL on failure); *guard = the first byte that
   must never be touched (== one past the end of the data page). */
static unsigned char *map_guarded(long pg, unsigned char **guard) {
    unsigned char *base = mmap(NULL, 2 * pg, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED)
        return NULL;
    if (mprotect(base + pg, pg, PROT_NONE) != 0) {
        munmap(base, 2 * pg);
        return NULL;
    }
    *guard = base + pg;
    return base;
}

int main(void) {
    long pg = sysconf(_SC_PAGESIZE);
    if (pg <= 0) pg = 4096;

    unsigned char *sg, *src_page = map_guarded(pg, &sg);   /* source + guard */
    unsigned char *dg, *dst_page = map_guarded(pg, &dg);   /* dest   + guard */
    if (!src_page || !dst_page) { perror("mmap/mprotect"); return 2; }

    for (long i = 0; i < pg; i++) {
        src_page[i] = (unsigned char)(i * 7 + 1);
        dst_page[i] = 0;
    }
    unsigned char *scratch = malloc(pg + 64);
    if (!scratch) { perror("malloc"); return 2; }

    struct sigaction sa, old;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = on_segv;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGSEGV, &sa, &old) != 0)
        fprintf(stderr, "warning: sigaction(SIGSEGV) failed; a fault will hard-crash\n");

    const int MAXLEN = (pg < 1024) ? (int) pg : 1024;
    int overread = 0, overwrite = 0, miscopy = 0, cases = 0;

    printf("memcpy_guard: page=%ld; sweeping len 1..%d at a PROT_NONE boundary\n",
           pg, MAXLEN);
    printf("  (if this hard-crashes to the Grim Reaper, clib4 did not deliver\n"
           "   SIGSEGV and the crash itself is the over-read.)\n");

    /* ---- Phase A: source ends AT the guard -> catch tail OVER-READ ---- */
    for (int len = 1; len <= MAXLEN; len++) {
        unsigned char *src = sg - len;             /* src + len == guard */
        cases++;
        if ((len & 63) == 0) { fprintf(stderr, "A len=%d\n", len); }
        g_faulted = 0;
        if (sigsetjmp(g_jmp, 1) == 0) {
            memcpy(scratch, src, len);
            if (memcmp(scratch, src, len) != 0) {
                miscopy++;
                printf("[FAIL] memcpy wrong result, len=%d\n", len);
            }
        } else {
            overread++;
            printf("[FAIL] memcpy OVER-READ past source end, len=%d (src&15=%ld)\n",
                   len, (long) ((uintptr_t) src & 15));
        }
        g_faulted = 0;
        if (sigsetjmp(g_jmp, 1) == 0) {
            memmove(scratch, src, len);            /* bcopy/memmove -> same asm */
        } else {
            overread++;
            printf("[FAIL] memmove OVER-READ past source end, len=%d\n", len);
        }
    }

    /* ---- Phase B: dest ends AT the guard -> catch tail OVER-WRITE ---- */
    for (int len = 1; len <= MAXLEN; len++) {
        unsigned char *dst = dg - len;             /* dst + len == guard */
        cases++;
        if ((len & 63) == 0) { fprintf(stderr, "B len=%d\n", len); }
        g_faulted = 0;
        if (sigsetjmp(g_jmp, 1) == 0) {
            memcpy(dst, src_page, len);
            if (memcmp(dst, src_page, len) != 0) {
                miscopy++;
                printf("[FAIL] memcpy wrong result (B), len=%d\n", len);
            }
        } else {
            overwrite++;
            printf("[FAIL] memcpy OVER-WRITE past dest end, len=%d (dst&15=%ld)\n",
                   len, (long) ((uintptr_t) dst & 15));
        }
    }

    sigaction(SIGSEGV, &old, NULL);
    printf("memcpy_guard: %d cases, over-read=%d over-write=%d miscopy=%d\n",
           cases, overread, overwrite, miscopy);
    int ok = (overread == 0 && overwrite == 0 && miscopy == 0);
    printf("memcpy_guard RESULT: %s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
