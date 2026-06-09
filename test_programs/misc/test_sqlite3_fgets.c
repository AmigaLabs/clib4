/*
 * Diagnostic test mimicking sqlite3's exact I/O pattern:
 *   fputs(prompt, stdout) → fflush(stdout) → fgets(buf, 100, stdin)
 *
 * Also checks console mode and detects RAW vs cooked.
 * Uses AmigaDOS Write(ErrorOutput(),...) for diagnostics to bypass stdio.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __amigaos4__
#include <unistd.h>
#define __USE_INLINE__
#include <proto/dos.h>
#include <proto/exec.h>
#include <dos/dos.h>

/* Debug via stderr fd — bypasses stdio */
static void dbg(const char *msg) {
    BPTR err = ErrorOutput();
    if (err != ZERO)
        Write(err, (CONST_STRPTR)msg, strlen(msg));
}

static void dbg_int(const char *prefix, int val) {
    char tmp[32];
    int i = sizeof(tmp) - 1;
    int neg = 0;
    unsigned int uval;

    dbg(prefix);
    if (val < 0) { neg = 1; uval = (unsigned int)(-val); }
    else { uval = (unsigned int)val; }
    tmp[i] = '\0';
    if (uval == 0) { tmp[--i] = '0'; }
    else { while (uval > 0 && i > 0) { tmp[--i] = '0' + (uval % 10); uval /= 10; } }
    if (neg && i > 0) tmp[--i] = '-';
    dbg(&tmp[i]);
}

static void dbg_hex(const char *prefix, unsigned int val) {
    char tmp[12];
    const char *hex = "0123456789abcdef";
    int i = sizeof(tmp) - 1;
    tmp[i] = '\0';
    if (val == 0) { tmp[--i] = '0'; }
    else { while (val > 0 && i > 0) { tmp[--i] = hex[val & 0xf]; val >>= 4; } }
    tmp[--i] = 'x';
    tmp[--i] = '0';
    dbg(prefix);
    dbg(&tmp[i]);
}

/* Check if console is in RAW mode by trying WaitForChar with 0 timeout.
 * In cooked mode, Read blocks until Enter. In RAW mode, characters arrive
 * individually. We can also try to detect the mode by examining the console
 * type via ObtainConsoleDataTags. */
static void check_console_mode(void) {
    BPTR fh_in  = Input();
    BPTR fh_out = Output();

    dbg("  Input()  = ");
    dbg_hex("", (unsigned int)fh_in);
    dbg("\n");

    dbg("  Output() = ");
    dbg_hex("", (unsigned int)fh_out);
    dbg("\n");

    if (fh_in != ZERO) {
        LONG interactive = IsInteractive(fh_in);
        dbg("  IsInteractive(Input()) = ");
        dbg_int("", interactive);
        dbg("\n");

        /* Test: WaitForChar with 0 timeout.
         * In cooked mode this returns 0 (no data pending).
         * In RAW mode this also returns 0 if no key is pressed.
         * Not a reliable mode detector, but useful for diagnostics. */
        LONG wfc = WaitForChar(fh_in, 0);
        dbg("  WaitForChar(Input(), 0) = ");
        dbg_int("", wfc);
        dbg("\n");
    }
}

/* Mimic sqlite3's local_getline exactly */
static char *local_getline(char *zLine, FILE *in) {
    int nLine = (zLine == 0) ? 0 : 100;
    int n = 0;

    while (1) {
        if (n + 100 > nLine) {
            nLine = nLine * 2 + 100;
            zLine = realloc(zLine, nLine);
            if (zLine == 0) return 0;
        }

        dbg("    local_getline: calling fgets(buf+");
        dbg_int("", n);
        dbg(", ");
        dbg_int("", nLine - n);
        dbg(", stdin)\n");

        if (fgets(&zLine[n], nLine - n, in) == 0) {
            dbg("    local_getline: fgets returned NULL\n");
            if (n == 0) {
                free(zLine);
                return 0;
            }
            zLine[n] = 0;
            break;
        }

        /* Show raw bytes returned by fgets */
        {
            int fgets_len = strlen(&zLine[n]);
            dbg("    local_getline: fgets returned ");
            dbg_int("", fgets_len);
            dbg(" bytes: ");
            for (int j = 0; j < fgets_len && j < 20; j++) {
                dbg_hex("", (unsigned char)zLine[n + j]);
                dbg(" ");
            }
            dbg("\n");
        }

        while (zLine[n]) n++;

        if (n > 0 && zLine[n - 1] == '\n') {
            n--;
            if (n > 0 && zLine[n - 1] == '\r') n--;
            zLine[n] = 0;
            dbg("    local_getline: found newline, line = '");
            dbg(zLine);
            dbg("'\n");
            break;
        }

        dbg("    local_getline: no newline found, looping...\n");
    }

    return zLine;
}

int main(int argc, char *argv[]) {
    char *zLine = NULL;
    int iteration = 0;
    int use_fputs = 1;  /* default: use fputs like sqlite3 */
    int open_file = 0;  /* optionally open a file first */
    int call_ioctl = 0; /* optionally call ioctl(TIOCGWINSZ) */
    int call_isatty_flag = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--printf") == 0) use_fputs = 0;
        if (strcmp(argv[i], "--open") == 0) open_file = 1;
        if (strcmp(argv[i], "--ioctl") == 0) call_ioctl = 1;
        if (strcmp(argv[i], "--isatty") == 0) call_isatty_flag = 1;
    }

    dbg("=== sqlite3-like fgets test ===\n");
    check_console_mode();

    if (use_fputs) dbg("  Mode: fputs (like sqlite3)\n");
    else           dbg("  Mode: printf (like simple test)\n");

    /* Mimic sqlite3's isatty calls */
    if (call_isatty_flag) {
        dbg("  Calling isatty(0)...\n");
        int r0 = isatty(0);
        dbg("  isatty(0) = ");
        dbg_int("", r0);
        dbg("\n");

        dbg("  Calling isatty(1)...\n");
        int r1 = isatty(1);
        dbg("  isatty(1) = ");
        dbg_int("", r1);
        dbg("\n");
    }

    /* Mimic sqlite3 opening a database file */
    if (open_file) {
        dbg("  Opening T:test_dummy.db...\n");
        FILE *f = fopen("T:test_dummy.db", "w+");
        if (f) {
            fprintf(f, "test data\n");
            fclose(f);
            dbg("  File opened and closed\n");
        } else {
            dbg("  File open FAILED\n");
        }
        check_console_mode();
    }

    /* Mimic sqlite3 calling ioctl for terminal width */
    if (call_ioctl) {
        dbg("  Calling ioctl(TIOCGWINSZ)...\n");
        #include <sys/ioctl.h>
        struct winsize ws;
        int r = ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
        dbg("  ioctl returned ");
        dbg_int("", r);
        dbg(", cols=");
        dbg_int("", ws.ws_col);
        dbg(", rows=");
        dbg_int("", ws.ws_row);
        dbg("\n");
        dbg("  Console mode after ioctl:\n");
        check_console_mode();
    }

    /* Mimic sqlite3 signal handler registration */
    /* signal(SIGINT, SIG_DFL); — not relevant */

    /* Main loop: prompt → fgets */
    while (1) {
        iteration++;
        dbg("\n--- iteration ");
        dbg_int("", iteration);
        dbg(" ---\n");

        const char *prompt = (iteration == 1) ? "sqlite> " : "   ...> ";

        if (use_fputs) {
            dbg("  [1] fputs(prompt, stdout)\n");
            fputs(prompt, stdout);
        } else {
            dbg("  [1] printf(prompt)\n");
            printf("%s", prompt);
        }

        dbg("  [2] fflush(stdout)\n");
        fflush(stdout);

        dbg("  [3] calling local_getline (sqlite3 style)...\n");
        zLine = local_getline(zLine, stdin);

        if (zLine == NULL) {
            dbg("  [4] local_getline returned NULL (EOF)\n");
            break;
        }

        dbg("  [4] local_getline returned: '");
        dbg(zLine);
        dbg("'\n");
        dbg_int("  [5] length = ", (int)strlen(zLine));
        dbg("\n");

        if (strlen(zLine) == 0) {
            dbg("  Empty line, showing next prompt\n");
            if (zLine) { free(zLine); zLine = NULL; }
            continue;
        }

        /* Show the result via output (like sqlite3 would print SQL results) */
        dbg("  [6] printing result via fprintf...\n");
        fprintf(stdout, "echo: '%s'\n", zLine);
        fflush(stdout);

        if (strcmp(zLine, "quit") == 0 || strcmp(zLine, ".quit") == 0) {
            dbg("  Exiting.\n");
            free(zLine);
            break;
        }

        /* Don't free zLine — reuse it like sqlite3 does */
    }

    dbg("=== test finished ===\n");
    return 0;
}

#else
/* Non-AmigaOS fallback */
#include <unistd.h>
#define dbg(msg) write(STDERR_FILENO, msg, strlen(msg))
int main(void) {
    dbg("This test is AmigaOS-specific\n");
    return 1;
}
#endif
