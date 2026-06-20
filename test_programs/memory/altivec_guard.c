/*
 * altivec_guard.c -- expose tail over-read / over-write in clib4's AltiVec
 * string/mem routines (the ones actually selected on a G4).
 *
 * clib4.c (libOpen) routes, on an AltiVec CPU:
 *     strcpy -> vec_strcpy   memcmp -> vec_memcmp
 *     bzero  -> vec_bzero    bcopy  -> vec_bcopy
 * (memcpy/memmove are NOT AltiVec-routed, which is why a memcpy-only test
 *  passes -- see memcpy_guard.c.)  These hand-written routines
 * (library/cpu/altivec/*.sx) use the classic "load two vectors + vperm"
 * unaligned idiom, which can touch up to 15 bytes past the end of a buffer.
 * Harmless mid-heap, but it FAULTS (DSI) when the buffer ends just before an
 * unmapped page -- e.g. a class-name UTF-8 string compared during VM class
 * loading, which crashed JamVM on the QEMU 'amigaone' (G4) target.
 *
 * This test puts the READ buffer (bcopy/memcmp/strcpy source) and the WRITE
 * buffer (bzero dest) so they end EXACTLY at a PROT_NONE guard page, so any
 * access one byte past the requested length faults.  A SIGSEGV handler +
 * siglongjmp lets the sweep continue and name the offending routine+length.
 * If clib4 does not deliver SIGSEGV, the process hard-crashes at the first
 * offending case (Grim Reaper) -- itself the proof; the stderr breadcrumb
 * (printed every 64 lengths) narrows it down.
 *
 * Only meaningful on AltiVec CPUs (G4 and the QEMU 'amigaone' machine).  On
 * non-AltiVec CPUs (G3, 440/460, X5000/P5020, A1222/P1022) clib4 uses scalar
 * routines and this should PASS.
 *
 * Build: drop in test_programs/memory/; `make compile-tests` builds it with
 *        -mcrt=clib4 -fno-builtin.  PASS == rc 0.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>      /* bcopy, bzero */
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
    siglongjmp(g_jmp, 1);
}

static unsigned char *map_guarded(long pg, unsigned char **guard) {
    unsigned char *base = mmap(NULL, 2 * pg, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED) return NULL;
    if (mprotect(base + pg, pg, PROT_NONE) != 0) { munmap(base, 2 * pg); return NULL; }
    *guard = base + pg;
    return base;
}

/* Print+flush a breadcrumb BEFORE each call so that, if clib4 does not deliver
   SIGSEGV and the process hard-crashes, the LAST line in the output names the
   exact routine + length that over-ran the guard. */
#define TRY(label, expr)                                                       \
    do {                                                                       \
        printf("> %s len=%d (buf&15=%ld)\n", label, len, (long)(boundary & 15));\
        fflush(stdout);                                                        \
        g_faulted = 0;                                                         \
        if (sigsetjmp(g_jmp, 1) == 0) { expr; }                                \
        else { fails++; printf("[FAIL] %s OVER-RAN guard, len=%d (buf&15=%ld)\n",\
                               label, len, (long)(boundary & 15)); }           \
    } while (0)

int main(void) {
    long pg = sysconf(_SC_PAGESIZE);
    if (pg <= 0) pg = 4096;

    unsigned char *rg, *rpage = map_guarded(pg, &rg);   /* read buffer + guard */
    unsigned char *wg, *wpage = map_guarded(pg, &wg);   /* write buffer + guard */
    if (!rpage || !wpage) { perror("mmap/mprotect"); return 2; }

    /* read page: non-zero everywhere (so strcpy only stops at the NUL we plant) */
    for (long i = 0; i < pg; i++) rpage[i] = (unsigned char)((i % 255) + 1);

    unsigned char *safe = malloc(pg + 64);     /* slack buffer for dst / cmp */
    if (!safe) { perror("malloc"); return 2; }
    memset(safe, 0x5A, pg + 64);

    struct sigaction sa, old;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = on_segv;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGSEGV, &sa, &old) != 0)
        fprintf(stderr, "warning: sigaction(SIGSEGV) failed; a fault hard-crashes\n");

    const int MAXLEN = (pg < 1024) ? (int) pg - 1 : 1024;
    int fails = 0;
    uintptr_t boundary;

    printf("altivec_guard: page=%ld; sweeping len 1..%d at a PROT_NONE boundary\n",
           pg, MAXLEN);
    printf("  routines under test (AltiVec on G4): bcopy memcmp strcpy bzero\n");
    printf("  (a hard crash to the Grim Reaper instead of a report = clib4 did\n"
           "   not deliver SIGSEGV; the crash itself is the over-run.)\n");

    for (int len = 1; len <= MAXLEN; len++) {
        if ((len & 63) == 0) fprintf(stderr, "len=%d\n", len);

        /* ---- bcopy: source ends at guard (over-READ) ---- */
        unsigned char *rsrc = rg - len;
        boundary = (uintptr_t) rsrc;
        TRY("bcopy",  bcopy(rsrc, safe, len));

        /* ---- memcmp: 2nd buffer ends at guard; make them equal so all len
                bytes are read (memcmp stops at first diff) (over-READ) ---- */
        memcpy(safe, rsrc, len);                 /* scalar memcpy -> safe */
        boundary = (uintptr_t) rsrc;
        TRY("memcmp", (void) memcmp(safe, rsrc, len));

        /* ---- strcpy: NUL-terminated string ends at guard (over-READ) ---- */
        rg[-1] = 0;                              /* plant NUL at last readable byte */
        {
            char *ssrc = (char *) (rg - 1 - len); /* len non-zero chars + NUL@rg-1 */
            boundary = (uintptr_t) ssrc;
            TRY("strcpy", strcpy((char *) safe, ssrc));
        }
        rg[-1] = (unsigned char)((((pg - 1) % 255) + 1)); /* restore non-zero */

        /* ---- bzero: dest ends at guard (over-WRITE) ---- */
        unsigned char *wdst = wg - len;
        boundary = (uintptr_t) wdst;
        TRY("bzero",  bzero(wdst, len));
    }

    sigaction(SIGSEGV, &old, NULL);
    printf("altivec_guard: %d failures across bcopy/memcmp/strcpy/bzero\n", fails);
    printf("altivec_guard RESULT: %s\n", fails == 0 ? "PASS" : "FAIL");
    return fails == 0 ? 0 : 1;
}
