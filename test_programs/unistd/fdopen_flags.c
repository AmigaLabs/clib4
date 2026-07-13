/*
 * fdopen_flags.c — Test that fdopen() sets read/write permissions correctly
 * for all mode strings, and that fcntl(F_GETFL) + O_ACCMODE works as expected.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define TMPFILE "T:fdopen_flags_test.tmp"

static int pass = 0;
static int fail = 0;

static void check(const char *desc, int ok) {
    if (ok) {
        printf("  PASS: %s\n", desc);
        pass++;
    } else {
        printf("  FAIL: %s\n", desc);
        fail++;
    }
}

/*
 * Try to read from f. The file must have been seeded with data beforehand.
 * Returns 1 if data was actually read (stream is readable),
 *         0 if fread returned 0 with ferror set (stream is not readable).
 */
static int try_fread(FILE *f) {
    char buf[16];
    size_t n = fread(buf, 1, sizeof(buf), f);
    int err = ferror(f);
    clearerr(f);
    if (n > 0) return 1;   /* readable: got data */
    if (err)   return 0;   /* not readable: stream error (EBADF) */
    /* n==0 and no ferror: ambiguous (should not happen if file was seeded) */
    return 0;
}

static int try_fwrite(FILE *f) {
    /* rewind to avoid O_APPEND position issues on write test */
    size_t n = fwrite("x", 1, 1, f);
    fflush(f);
    int ok = (n == 1);
    clearerr(f);
    return ok;
}

/*
 * Test a specific fdopen mode.
 *   expect_read  : 1 if fread should succeed
 *   expect_write : 1 if fwrite should succeed
 */
static void test_mode(const char *mode, int expect_read, int expect_write) {
    char desc[64];
    int flags;
    FILE *f;

    printf("\n--- mode \"%s\" (expect read=%d write=%d) ---\n",
           mode, expect_read, expect_write);

    /* Choose the right open flags to match the mode. */
    switch (mode[0]) {
        case 'r': flags = O_RDONLY; break;
        case 'w': flags = O_WRONLY | O_CREAT | O_TRUNC; break;
        case 'a': flags = O_WRONLY | O_CREAT | O_APPEND; break;
        default:  flags = O_RDWR | O_CREAT | O_TRUNC; break;
    }
    if (mode[1] == '+' || (mode[1] != '\0' && mode[2] == '+'))
        flags = (O_RDWR | (flags & ~(O_RDONLY | O_WRONLY)));

    /*
     * For 'r' modes the file must exist before open(O_RDONLY).
     * Seed via a separate fd first so the file is non-empty.
     */
    if (mode[0] == 'r') {
        int seed_fd = open(TMPFILE, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (seed_fd < 0) {
            printf("  SKIP: seed open failed for mode \"%s\": %s\n", mode, strerror(errno));
            return;
        }
        write(seed_fd, "hello", 5);
        close(seed_fd);
    }

    int fd = open(TMPFILE, flags, 0600);
    if (fd < 0) {
        printf("  SKIP: open() failed for mode \"%s\": %s\n", mode, strerror(errno));
        unlink(TMPFILE);
        return;
    }

    /*
     * Seed data into the file via the raw fd for all modes so that
     * try_fread() can distinguish "readable and got data" from
     * "not readable (ferror)" vs "empty file (ambiguous)".
     * For append mode seek to start after writing so fread can find data.
     */
    write(fd, "hello", 5);
    lseek(fd, 0, SEEK_SET);

    f = fdopen(fd, mode);
    snprintf(desc, sizeof(desc), "fdopen(\"%s\") returns non-NULL", mode);
    check(desc, f != NULL);
    if (f == NULL) {
        close(fd);
        return;
    }

    /* ---- Test read ---- */
    int got_read = try_fread(f);
    snprintf(desc, sizeof(desc), "fread  on mode \"%s\" %s",
             mode, expect_read ? "succeeds" : "fails");
    check(desc, got_read == expect_read);

    /* Rewind so write test starts from a known position. */
    rewind(f);

    /* ---- Test write ---- */
    int got_write = try_fwrite(f);
    snprintf(desc, sizeof(desc), "fwrite on mode \"%s\" %s",
             mode, expect_write ? "succeeds" : "fails");
    check(desc, got_write == expect_write);

    fclose(f);
    unlink(TMPFILE);
}

/*
 * Test that fcntl(F_GETFL) + (O_ACCMODE) switch works correctly — this is
 * the pattern used by BFD / binutils.
 */
static void test_fcntl_accmode(void) {
    printf("\n--- fcntl(F_GETFL) + O_ACCMODE switch ---\n");

    struct {
        int oflags;
        int expected_acc;
        const char *label;
    } cases[] = {
        { O_RDONLY,                        O_RDONLY, "O_RDONLY" },
        { O_WRONLY | O_CREAT | O_TRUNC,    O_WRONLY, "O_WRONLY" },
        { O_RDWR   | O_CREAT | O_TRUNC,    O_RDWR,   "O_RDWR"   },
    };

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        /* Seed file for O_RDONLY */
        if ((cases[i].oflags & O_ACCMODE) == O_RDONLY) {
            int seed = open(TMPFILE, O_WRONLY | O_CREAT | O_TRUNC, 0600);
            if (seed >= 0) { write(seed, "x", 1); close(seed); }
        }

        int fd = open(TMPFILE, cases[i].oflags, 0600);
        if (fd < 0) {
            printf("  SKIP: open failed for %s: %s\n", cases[i].label, strerror(errno));
            continue;
        }

        int fdflags = fcntl(fd, F_GETFL, 0);
        char desc[64];
        snprintf(desc, sizeof(desc),
                 "fcntl(F_GETFL) succeeds for %s", cases[i].label);
        check(desc, fdflags != -1);

        if (fdflags != -1) {
            int acc = fdflags & O_ACCMODE;
            snprintf(desc, sizeof(desc),
                     "O_ACCMODE mask == %s (got %d, want %d)",
                     cases[i].label, acc, cases[i].expected_acc);
            check(desc, acc == cases[i].expected_acc);

            /* Replicate the BFD switch */
            int switch_ok = 0;
            switch (acc) {
                case O_RDONLY: switch_ok = (cases[i].expected_acc == O_RDONLY); break;
                case O_WRONLY: switch_ok = (cases[i].expected_acc == O_WRONLY); break;
                case O_RDWR:   switch_ok = (cases[i].expected_acc == O_RDWR);   break;
                default:       switch_ok = 0; break;
            }
            snprintf(desc, sizeof(desc),
                     "BFD-style switch matches for %s", cases[i].label);
            check(desc, switch_ok);
        }

        close(fd);
        unlink(TMPFILE);
    }
}

int main(void) {
    printf("=== fdopen() flag correctness tests ===\n");

    /* Single-access modes: read XOR write */
    test_mode("r",  /*read*/1, /*write*/0);
    test_mode("w",  /*read*/0, /*write*/1);
    test_mode("a",  /*read*/0, /*write*/1);

    /* Dual-access modes: both read AND write */
    test_mode("r+", /*read*/1, /*write*/1);
    test_mode("w+", /*read*/1, /*write*/1);
    test_mode("a+", /*read*/1, /*write*/1);

    test_fcntl_accmode();

    printf("\n=== Results: %d passed, %d failed ===\n", pass, fail);
    return (fail == 0) ? 0 : 1;
}
