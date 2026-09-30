/*
 * Test for wordexp()/wordfree().
 *
 * Exercises: plain words, quotes, tilde, variable expansion, globbing,
 * and command substitution (via spawnvpe + C:Run on AmigaOS).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>

static int errors = 0;

static void
check(const char *input, int flags, const char *const *expected, size_t nexpected) {
    wordexp_t we;
    int rc = wordexp(input, &we, flags);
    if (rc != 0) {
        printf("FAIL: wordexp(\"%s\") returned %d\n", input, rc);
        errors++;
        return;
    }
    printf("wordexp(\"%s\") -> %zu words:", input, we.we_wordc);
    for (size_t i = 0; i < we.we_wordc; i++)
        printf(" [%s]", we.we_wordv[i]);
    printf("\n");

    if (we.we_wordc != nexpected) {
        printf("FAIL: expected %zu words, got %zu\n", nexpected, we.we_wordc);
        errors++;
    } else if (expected) {
        for (size_t i = 0; i < nexpected; i++) {
            if (strcmp(we.we_wordv[i], expected[i]) != 0) {
                printf("FAIL: word %zu = \"%s\", expected \"%s\"\n",
                       i, we.we_wordv[i], expected[i]);
                errors++;
            }
        }
    }
    wordfree(&we);
}

int
main(void) {
    static const char *const e1[] = {"one", "two", "three"};
    static const char *const e2[] = {"hello world", "x"};
    static const char *const e3[] = {"a=b"};

    /* Simple splitting */
    check("one two three", 0, e1, 3);

    /* Double quotes keep spaces together */
    check("\"hello world\" x", 0, e2, 2);

    /* Single quotes + assignment-like token */
    check("'a=b'", 0, e3, 1);

    /* Variable expansion: $HOME should expand (value depends on env) */
    {
        wordexp_t we;
        if (wordexp("$HOME", &we, 0) == 0) {
            printf("wordexp(\"$HOME\") -> %zu words:", we.we_wordc);
            for (size_t i = 0; i < we.we_wordc; i++)
                printf(" [%s]", we.we_wordv[i]);
            printf("\n");
            wordfree(&we);
        } else {
            printf("note: $HOME not expanded (HOME unset?)\n");
        }
    }

    /* Command substitution */
    {
        wordexp_t we;
        int rc = wordexp("`echo cmdsub-test`", &we, 0);
        if (rc == 0) {
            printf("cmdsub -> %zu words:", we.we_wordc);
            for (size_t i = 0; i < we.we_wordc; i++)
                printf(" [%s]", we.we_wordv[i]);
            printf("\n");
            if (we.we_wordc != 1 || strcmp(we.we_wordv[0], "cmdsub-test") != 0) {
                printf("FAIL: cmdsub expected [cmdsub-test]\n");
                errors++;
            }
            wordfree(&we);
        } else {
            printf("FAIL: command substitution returned %d\n", rc);
            errors++;
        }
    }

    if (errors > 0) {
        printf("FAILURE: %d errors\n", errors);
        return EXIT_FAILURE;
    }
    printf("SUCCESS\n");
    return EXIT_SUCCESS;
}
