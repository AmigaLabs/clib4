/*
 * Checks the placeholder names clib4 defines for Linux compatibility: their
 * values, how the mask names are composed, and that no placeholder bit overlaps
 * a bit clib4 uses. Placeholders compile but have no effect, so this test does
 * not check behaviour; it checks the definitions a port relies on.
 *
 * The test is built with -DNDEBUG, so it uses explicit checks, not assert().
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>

static int failures = 0;

static void check(int ok, const char *what) {
    printf("%s: %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok)
        failures++;
}

int main(void) {
    unsigned long defined;
    unsigned long placeholders;
    unsigned long sum;

    /* Baud rates use the rate itself as the constant, as clib4 does. */
    check(B9600 == 9600, "B9600 is the rate 9600");
    check(B921600 == 921600, "B921600 is the rate 921600");
    check(B4000000 == 4000000, "B4000000 is the rate 4000000");

    /* Output flag placeholders: masks are composed from their single bits. */
    check(NL0 == 0 && CR0 == 0 && TAB0 == 0 && BS0 == 0 && FF0 == 0 && VT0 == 0,
          "zero-valued selectors are 0");
    check(NLDLY == NL1, "NLDLY is NL1");
    check(CR3 == (CR1 | CR2) && CRDLY == CR3, "CR3 and CRDLY are CR1|CR2");
    check(TAB3 == (TAB1 | TAB2) && TABDLY == TAB3, "TAB3 and TABDLY are TAB1|TAB2");
    check(BSDLY == BS1, "BSDLY is BS1");
    check(FFDLY == FF1, "FFDLY is FF1");
    check(VTDLY == VT1, "VTDLY is VT1");

    /* The single-bit output placeholders must not overlap each other or the
     * output flags clib4 defines (OPOST, ONLCR, OCRNL, ONOCR, ONLRET). */
    defined = OPOST | ONLCR | OCRNL | ONOCR | ONLRET;
    placeholders = NL1 | BS1 | FF1 | VT1 | CR1 | CR2 | TAB1 | TAB2 | OFDEL | OFILL | OLCUC | XTABS;
    sum = (unsigned long)NL1 + BS1 + FF1 + VT1 + CR1 + CR2 + TAB1 + TAB2 + OFDEL + OFILL + OLCUC + XTABS;
    check(placeholders == sum, "output placeholder bits do not overlap each other");
    check((defined & placeholders) == 0, "output placeholder bits do not overlap clib4 output flags");

    /* Local flag placeholders must not overlap clib4's local flags. */
    defined = ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHONL | NOFLSH | TOSTOP | IEXTEN | FLUSHO | ECHOKE | ECHOCTL;
    placeholders = ECHOPRT | PENDIN | EXTPROC;
    sum = (unsigned long)ECHOPRT + PENDIN + EXTPROC;
    check(placeholders == sum, "local flag placeholder bits do not overlap each other");
    check((defined & placeholders) == 0, "local flag placeholder bits do not overlap clib4 local flags");

    /* Control-character indices are the values the names stand for. */
    check(VREPRINT == 12 && VDISCARD == 13 && VLNEXT == 14 && VSWTC == 15,
          "control-character placeholders use indices 12 to 15");
    check(VEOL2 == 16, "VEOL2 is index 16 (outside c_cc[NCCS]; not to be stored)");

    /* errno placeholders start above clib4's own range, which ends at 89. */
    check(EDEADLOCK == EDEADLK, "EDEADLOCK is an alias of EDEADLK");
    check(EADV == 200 && EXFULL == 251, "errno placeholders run from 200");
    check(EADV > ENOTSUP && EXFULL > ENOTSUP, "errno placeholders are above clib4's range");

    /* ioctl and fcntl values. */
    check(TIOCEXCL == 0x540C && TIOCNXCL == 0x540D, "TIOCEXCL and TIOCNXCL have the Linux values");
    check(F_SETFD == 2 && F_DUPFD_CLOEXEC == 1030, "F_SETFD and F_DUPFD_CLOEXEC have the expected values");

    printf("%s: %d failure(s)\n", failures ? "RESULT FAIL" : "RESULT OK", failures);
    return failures ? 1 : 0;
}
