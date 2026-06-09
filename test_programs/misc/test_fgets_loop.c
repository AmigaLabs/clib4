/*
 * Detailed test: printf prompt + fgets in a loop.
 * Tests whether fgets blocks on the second call.
 *
 * Uses AmigaOS Write() directly for debug output to avoid
 * any interaction with the stdio layer being tested.
 *
 * Expected behavior:
 *   prompt> hello
 *   got: 'hello'
 *   prompt> world
 *   got: 'world'
 *   prompt> quit
 *   Bye!
 */
#include <stdio.h>
#include <string.h>

#ifdef __amigaos4__
#include <unistd.h>

/* Debug print via POSIX write() to stderr fd — bypasses stdio completely.
 * IMPORTANT: dbg_int/dbg_hex use hand-rolled int-to-string to avoid
 * calling snprintf which goes through the stdio layer we're testing. */
static void dbg(const char *msg) {
    write(STDERR_FILENO, msg, strlen(msg));
}

static void dbg_int(const char *prefix, int val, const char *suffix) {
    char tmp[32];
    int neg = 0;
    unsigned int uval;
    int i;

    dbg(prefix);

    if (val < 0) { neg = 1; uval = (unsigned int)(-val); }
    else { uval = (unsigned int)val; }

    i = sizeof(tmp) - 1;
    tmp[i] = '\0';
    if (uval == 0) { tmp[--i] = '0'; }
    else { while (uval > 0 && i > 0) { tmp[--i] = '0' + (uval % 10); uval /= 10; } }
    if (neg && i > 0) tmp[--i] = '-';

    dbg(&tmp[i]);
    dbg(suffix);
}

static void dbg_hex(const char *prefix, unsigned long val, const char *suffix) {
    char tmp[20];
    const char hex[] = "0123456789abcdef";
    int i;

    dbg(prefix);

    i = sizeof(tmp) - 1;
    tmp[i] = '\0';
    if (val == 0) { tmp[--i] = '0'; }
    else { while (val > 0 && i > 0) { tmp[--i] = hex[val & 0xf]; val >>= 4; } }
    if (i > 1) { tmp[--i] = 'x'; tmp[--i] = '0'; }

    dbg(&tmp[i]);
    dbg(suffix);
}
#else
/* Non-AmigaOS fallback: use fprintf(stderr) */
static void dbg(const char *msg) {
    fputs(msg, stderr);
}
static void dbg_int(const char *prefix, int val, const char *suffix) {
    fprintf(stderr, "%s%d%s", prefix, val, suffix);
}
static void dbg_hex(const char *prefix, unsigned long val, const char *suffix) {
    fprintf(stderr, "%s0x%lx%s", prefix, val, suffix);
}
#endif

int main(void) {
    char buf[256];
    int i = 0;

    dbg("=== fgets loop test starting ===\n");
    dbg_hex("  stdin  = ", (unsigned long)stdin, "\n");
    dbg_hex("  stdout = ", (unsigned long)stdout, "\n");
    dbg_hex("  stderr = ", (unsigned long)stderr, "\n");

    while (1) {
        i++;

        dbg_int("\n--- iteration ", i, " ---\n");
        dbg("  [1] calling printf(\"prompt> \")\n");

        printf("prompt> ");

        dbg("  [2] printf returned\n");
        dbg("  [3] calling fflush(stdout)\n");

        fflush(stdout);

        dbg("  [4] fflush returned\n");
        dbg("  [5] calling fgets(buf, 256, stdin)\n");

        char *result = fgets(buf, sizeof(buf), stdin);

        if (result == NULL) {
            dbg("  [6] fgets returned NULL\n");
            if (feof(stdin))
                dbg("  [7] stdin is at EOF\n");
            if (ferror(stdin))
                dbg("  [7] stdin has error\n");
            break;
        }

        /* Show what fgets returned */
        dbg("  [6] fgets returned: '");
        dbg(buf);
        dbg("'\n");
        dbg_int("  [7] length = ", (int)strlen(buf), "\n");

        /* Show bytes for debugging CR/LF issues */
        {
            size_t len = strlen(buf);
            if (len > 0) {
                dbg("  [8] last bytes:");
                for (size_t j = (len > 4 ? len - 4 : 0); j < len; j++) {
                    dbg_hex(" ", (unsigned char)buf[j], "");
                }
                dbg("\n");
            }
        }

        /* Remove trailing newline */
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
            buf[len - 1] = '\0';

        dbg("  [9] calling printf(\"got: ...\")\n");
        printf("got: '%s'\n", buf);

        dbg("  [10] printf returned\n");

        if (strcmp(buf, "quit") == 0) {
            printf("Bye!\n");
            dbg("  Exiting on 'quit'\n");
            break;
        }
    }

    dbg("=== fgets loop test finished ===\n");
    return 0;
}
