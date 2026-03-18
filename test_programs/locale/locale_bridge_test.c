/*
 * locale_bridge_test.c — Comprehensive test for clib4 locale.library bridge
 *
 * Tests setlocale(), nl_langinfo(), and localeconv() improvements:
 * - setlocale(LC_ALL, "") activates system default locale
 * - setlocale(LC_ALL, "C") and "POSIX" work correctly
 * - nl_langinfo() returns localized day/month/AM/PM/yes/no strings
 * - nl_langinfo() returns YESEXPR/NOEXPR/CRNCYSTR/T_FMT_AMPM/ERA
 * - localeconv() returns locale-aware monetary/numeric data
 *
 * Build: ppc-amigaos-gcc -mcrt=clib4 -o locale_bridge_test locale_bridge_test.c
 * Run on AmigaOS 4.1: locale_bridge_test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <langinfo.h>

static int pass_count = 0;
static int fail_count = 0;

#define TEST(desc, cond) do { \
    if (cond) { \
        printf("  PASS: %s\n", desc); \
        pass_count++; \
    } else { \
        printf("  FAIL: %s\n", desc); \
        fail_count++; \
    } \
} while(0)

static void test_setlocale_query(void) {
    printf("\n=== setlocale() query tests ===\n");

    /* Query without changing should return current locale name */
    char *loc = setlocale(LC_ALL, NULL);
    TEST("setlocale(LC_ALL, NULL) returns non-NULL", loc != NULL);
    if (loc)
        printf("  INFO: Current locale = \"%s\"\n", loc);
}

static void test_setlocale_c_locale(void) {
    printf("\n=== setlocale() C locale tests ===\n");

    char *loc;

    loc = setlocale(LC_ALL, "C");
    TEST("setlocale(LC_ALL, \"C\") succeeds", loc != NULL);
    if (loc)
        printf("  INFO: C locale name = \"%s\"\n", loc);

    loc = setlocale(LC_ALL, "POSIX");
    TEST("setlocale(LC_ALL, \"POSIX\") succeeds", loc != NULL);
    if (loc)
        printf("  INFO: POSIX locale name = \"%s\"\n", loc);

    loc = setlocale(LC_ALL, "C-UTF-8");
    TEST("setlocale(LC_ALL, \"C-UTF-8\") succeeds", loc != NULL);
    if (loc)
        printf("  INFO: C-UTF-8 locale name = \"%s\"\n", loc);
}

static void test_setlocale_system_default(void) {
    printf("\n=== setlocale(\"\") system default tests ===\n");

    /* This is the critical test — setlocale(LC_ALL, "") should
     * activate the system's default locale from Locale prefs.
     */
    char *loc = setlocale(LC_ALL, "");
    TEST("setlocale(LC_ALL, \"\") succeeds (system default)", loc != NULL);
    if (loc) {
        printf("  INFO: System default locale = \"%s\"\n", loc);
        /* The name should contain an encoding suffix for wchar compat */
        TEST("Locale name contains encoding info (has '-')",
             strchr(loc, '-') != NULL);
    }
}

static void test_setlocale_categories(void) {
    printf("\n=== setlocale() per-category tests ===\n");

    /* First activate system default */
    setlocale(LC_ALL, "");

    char *loc;

    loc = setlocale(LC_NUMERIC, NULL);
    TEST("setlocale(LC_NUMERIC, NULL) returns non-NULL", loc != NULL);
    if (loc)
        printf("  INFO: LC_NUMERIC = \"%s\"\n", loc);

    loc = setlocale(LC_MONETARY, NULL);
    TEST("setlocale(LC_MONETARY, NULL) returns non-NULL", loc != NULL);
    if (loc)
        printf("  INFO: LC_MONETARY = \"%s\"\n", loc);

    loc = setlocale(LC_TIME, NULL);
    TEST("setlocale(LC_TIME, NULL) returns non-NULL", loc != NULL);
    if (loc)
        printf("  INFO: LC_TIME = \"%s\"\n", loc);

    loc = setlocale(LC_COLLATE, NULL);
    TEST("setlocale(LC_COLLATE, NULL) returns non-NULL", loc != NULL);
    if (loc)
        printf("  INFO: LC_COLLATE = \"%s\"\n", loc);
}

static void test_nl_langinfo_codeset(void) {
    printf("\n=== nl_langinfo(CODESET) tests ===\n");

    setlocale(LC_ALL, "");

    char *cs = nl_langinfo(CODESET);
    TEST("nl_langinfo(CODESET) returns non-NULL", cs != NULL);
    TEST("nl_langinfo(CODESET) returns non-empty", cs != NULL && cs[0] != '\0');
    if (cs)
        printf("  INFO: CODESET = \"%s\"\n", cs);
}

static void test_nl_langinfo_datetime(void) {
    printf("\n=== nl_langinfo() date/time format tests ===\n");

    setlocale(LC_ALL, "");

    char *s;

    s = nl_langinfo(D_T_FMT);
    TEST("nl_langinfo(D_T_FMT) returns non-NULL", s != NULL);
    if (s)
        printf("  INFO: D_T_FMT = \"%s\"\n", s);

    s = nl_langinfo(D_FMT);
    TEST("nl_langinfo(D_FMT) returns non-NULL", s != NULL);
    if (s)
        printf("  INFO: D_FMT = \"%s\"\n", s);

    s = nl_langinfo(T_FMT);
    TEST("nl_langinfo(T_FMT) returns non-NULL", s != NULL);
    if (s)
        printf("  INFO: T_FMT = \"%s\"\n", s);

    s = nl_langinfo(T_FMT_AMPM);
    TEST("nl_langinfo(T_FMT_AMPM) returns non-NULL", s != NULL);
    TEST("nl_langinfo(T_FMT_AMPM) returns non-empty",
         s != NULL && s[0] != '\0');
    if (s)
        printf("  INFO: T_FMT_AMPM = \"%s\"\n", s);
}

static void test_nl_langinfo_day_names(void) {
    printf("\n=== nl_langinfo() day name tests ===\n");

    setlocale(LC_ALL, "");

    /* Full day names (DAY_1=Sunday .. DAY_7=Saturday) */
    const nl_item day_items[] = {DAY_1, DAY_2, DAY_3, DAY_4,
                                  DAY_5, DAY_6, DAY_7};
    const char *day_labels[] = {"DAY_1(Sun)", "DAY_2(Mon)", "DAY_3(Tue)",
                                 "DAY_4(Wed)", "DAY_5(Thu)", "DAY_6(Fri)",
                                 "DAY_7(Sat)"};

    for (int i = 0; i < 7; i++) {
        char *s = nl_langinfo(day_items[i]);
        char desc[64];
        snprintf(desc, sizeof(desc), "nl_langinfo(%s) non-empty", day_labels[i]);
        TEST(desc, s != NULL && s[0] != '\0');
        if (s)
            printf("  INFO: %s = \"%s\"\n", day_labels[i], s);
    }

    /* Abbreviated day names */
    printf("\n  --- Abbreviated day names ---\n");
    const nl_item abday_items[] = {ABDAY_1, ABDAY_2, ABDAY_3, ABDAY_4,
                                    ABDAY_5, ABDAY_6, ABDAY_7};
    const char *abday_labels[] = {"ABDAY_1", "ABDAY_2", "ABDAY_3",
                                   "ABDAY_4", "ABDAY_5", "ABDAY_6",
                                   "ABDAY_7"};

    for (int i = 0; i < 7; i++) {
        char *s = nl_langinfo(abday_items[i]);
        char desc[64];
        snprintf(desc, sizeof(desc), "nl_langinfo(%s) non-empty", abday_labels[i]);
        TEST(desc, s != NULL && s[0] != '\0');
        if (s)
            printf("  INFO: %s = \"%s\"\n", abday_labels[i], s);
    }
}

static void test_nl_langinfo_month_names(void) {
    printf("\n=== nl_langinfo() month name tests ===\n");

    setlocale(LC_ALL, "");

    const nl_item mon_items[] = {MON_1, MON_2, MON_3, MON_4,
                                  MON_5, MON_6, MON_7, MON_8,
                                  MON_9, MON_10, MON_11, MON_12};
    const char *mon_labels[] = {"MON_1(Jan)", "MON_2(Feb)", "MON_3(Mar)",
                                 "MON_4(Apr)", "MON_5(May)", "MON_6(Jun)",
                                 "MON_7(Jul)", "MON_8(Aug)", "MON_9(Sep)",
                                 "MON_10(Oct)", "MON_11(Nov)", "MON_12(Dec)"};

    for (int i = 0; i < 12; i++) {
        char *s = nl_langinfo(mon_items[i]);
        char desc[64];
        snprintf(desc, sizeof(desc), "nl_langinfo(%s) non-empty", mon_labels[i]);
        TEST(desc, s != NULL && s[0] != '\0');
        if (s)
            printf("  INFO: %s = \"%s\"\n", mon_labels[i], s);
    }

    /* Abbreviated month names */
    printf("\n  --- Abbreviated month names ---\n");
    const nl_item abmon_items[] = {ABMON_1, ABMON_2, ABMON_3, ABMON_4,
                                    ABMON_5, ABMON_6, ABMON_7, ABMON_8,
                                    ABMON_9, ABMON_10, ABMON_11, ABMON_12};
    const char *abmon_labels[] = {"ABMON_1", "ABMON_2", "ABMON_3",
                                   "ABMON_4", "ABMON_5", "ABMON_6",
                                   "ABMON_7", "ABMON_8", "ABMON_9",
                                   "ABMON_10", "ABMON_11", "ABMON_12"};

    for (int i = 0; i < 12; i++) {
        char *s = nl_langinfo(abmon_items[i]);
        char desc[64];
        snprintf(desc, sizeof(desc), "nl_langinfo(%s) non-empty", abmon_labels[i]);
        TEST(desc, s != NULL && s[0] != '\0');
        if (s)
            printf("  INFO: %s = \"%s\"\n", abmon_labels[i], s);
    }
}

static void test_nl_langinfo_ampm(void) {
    printf("\n=== nl_langinfo() AM/PM tests ===\n");

    setlocale(LC_ALL, "");

    char *am = nl_langinfo(AM_STR);
    char *pm = nl_langinfo(PM_STR);

    TEST("nl_langinfo(AM_STR) returns non-NULL", am != NULL);
    TEST("nl_langinfo(AM_STR) returns non-empty", am != NULL && am[0] != '\0');
    TEST("nl_langinfo(PM_STR) returns non-NULL", pm != NULL);
    TEST("nl_langinfo(PM_STR) returns non-empty", pm != NULL && pm[0] != '\0');

    if (am) printf("  INFO: AM_STR = \"%s\"\n", am);
    if (pm) printf("  INFO: PM_STR = \"%s\"\n", pm);
}

static void test_nl_langinfo_yesno(void) {
    printf("\n=== nl_langinfo() yes/no tests ===\n");

    setlocale(LC_ALL, "");

    char *yes = nl_langinfo(YESSTR);
    char *no = nl_langinfo(NOSTR);

    TEST("nl_langinfo(YESSTR) returns non-NULL", yes != NULL);
    TEST("nl_langinfo(YESSTR) returns non-empty", yes != NULL && yes[0] != '\0');
    TEST("nl_langinfo(NOSTR) returns non-NULL", no != NULL);
    TEST("nl_langinfo(NOSTR) returns non-empty", no != NULL && no[0] != '\0');

    if (yes) printf("  INFO: YESSTR = \"%s\"\n", yes);
    if (no)  printf("  INFO: NOSTR = \"%s\"\n", no);

    /* YESEXPR / NOEXPR */
    char *yesexpr = nl_langinfo(YESEXPR);
    char *noexpr = nl_langinfo(NOEXPR);

    TEST("nl_langinfo(YESEXPR) returns non-NULL", yesexpr != NULL);
    TEST("nl_langinfo(YESEXPR) starts with '^'",
         yesexpr != NULL && yesexpr[0] == '^');
    TEST("nl_langinfo(NOEXPR) returns non-NULL", noexpr != NULL);
    TEST("nl_langinfo(NOEXPR) starts with '^'",
         noexpr != NULL && noexpr[0] == '^');

    if (yesexpr) printf("  INFO: YESEXPR = \"%s\"\n", yesexpr);
    if (noexpr)  printf("  INFO: NOEXPR = \"%s\"\n", noexpr);
}

static void test_nl_langinfo_currency(void) {
    printf("\n=== nl_langinfo() currency tests ===\n");

    setlocale(LC_ALL, "");

    char *crncystr = nl_langinfo(CRNCYSTR);
    TEST("nl_langinfo(CRNCYSTR) returns non-NULL", crncystr != NULL);
    if (crncystr && crncystr[0] != '\0') {
        TEST("CRNCYSTR starts with '-' or '+'",
             crncystr[0] == '-' || crncystr[0] == '+');
        printf("  INFO: CRNCYSTR = \"%s\" (%s value)\n", crncystr,
               crncystr[0] == '-' ? "precedes" : "succeeds");
    } else {
        printf("  INFO: CRNCYSTR is empty (no currency symbol configured)\n");
    }
}

static void test_nl_langinfo_numeric(void) {
    printf("\n=== nl_langinfo() numeric tests ===\n");

    setlocale(LC_ALL, "");

    char *radix = nl_langinfo(RADIXCHAR);
    char *thousep = nl_langinfo(THOUSEP);

    TEST("nl_langinfo(RADIXCHAR) returns non-NULL", radix != NULL);
    TEST("nl_langinfo(RADIXCHAR) returns non-empty",
         radix != NULL && radix[0] != '\0');
    TEST("nl_langinfo(THOUSEP) returns non-NULL", thousep != NULL);

    if (radix)   printf("  INFO: RADIXCHAR = \"%s\"\n", radix);
    if (thousep)  printf("  INFO: THOUSEP = \"%s\"\n", thousep);
}

static void test_nl_langinfo_era(void) {
    printf("\n=== nl_langinfo() ERA tests ===\n");

    setlocale(LC_ALL, "");

    /* ERA items should return empty strings (AmigaOS has no era support) */
    char *era = nl_langinfo(ERA);
    char *era_d_fmt = nl_langinfo(ERA_D_FMT);
    char *era_d_t_fmt = nl_langinfo(ERA_D_T_FMT);
    char *era_t_fmt = nl_langinfo(ERA_T_FMT);
    char *alt_digits = nl_langinfo(ALT_DIGITS);

    TEST("nl_langinfo(ERA) returns non-NULL", era != NULL);
    TEST("nl_langinfo(ERA_D_FMT) returns non-NULL", era_d_fmt != NULL);
    TEST("nl_langinfo(ERA_D_T_FMT) returns non-NULL", era_d_t_fmt != NULL);
    TEST("nl_langinfo(ERA_T_FMT) returns non-NULL", era_t_fmt != NULL);
    TEST("nl_langinfo(ALT_DIGITS) returns non-NULL", alt_digits != NULL);

    printf("  INFO: ERA = \"%s\"\n", era ? era : "(null)");
    printf("  INFO: ALT_DIGITS = \"%s\"\n", alt_digits ? alt_digits : "(null)");
}

static void test_localeconv(void) {
    printf("\n=== localeconv() tests ===\n");

    setlocale(LC_ALL, "");

    struct lconv *lc = localeconv();
    TEST("localeconv() returns non-NULL", lc != NULL);
    if (lc == NULL)
        return;

    TEST("decimal_point is non-NULL", lc->decimal_point != NULL);
    TEST("decimal_point is non-empty",
         lc->decimal_point != NULL && lc->decimal_point[0] != '\0');

    printf("  INFO: decimal_point = \"%s\"\n",
           lc->decimal_point ? lc->decimal_point : "(null)");
    printf("  INFO: thousands_sep = \"%s\"\n",
           lc->thousands_sep ? lc->thousands_sep : "(null)");
    printf("  INFO: currency_symbol = \"%s\"\n",
           lc->currency_symbol ? lc->currency_symbol : "(null)");
    printf("  INFO: int_curr_symbol = \"%s\"\n",
           lc->int_curr_symbol ? lc->int_curr_symbol : "(null)");
    printf("  INFO: mon_decimal_point = \"%s\"\n",
           lc->mon_decimal_point ? lc->mon_decimal_point : "(null)");
    printf("  INFO: positive_sign = \"%s\"\n",
           lc->positive_sign ? lc->positive_sign : "(null)");
    printf("  INFO: negative_sign = \"%s\"\n",
           lc->negative_sign ? lc->negative_sign : "(null)");
    printf("  INFO: frac_digits = %d\n", (int)(unsigned char)lc->frac_digits);
    printf("  INFO: p_cs_precedes = %d\n", (int)(unsigned char)lc->p_cs_precedes);
    printf("  INFO: p_sign_posn = %d\n", (int)(unsigned char)lc->p_sign_posn);
}

static void test_setlocale_restore(void) {
    printf("\n=== setlocale() C restore test ===\n");

    /* Switch back to C locale and verify */
    char *loc = setlocale(LC_ALL, "C");
    TEST("setlocale(LC_ALL, \"C\") succeeds after system default", loc != NULL);

    /* In C locale, localeconv should return C defaults */
    struct lconv *lc = localeconv();
    TEST("localeconv() in C locale returns non-NULL", lc != NULL);
    if (lc && lc->decimal_point) {
        TEST("C locale decimal_point is \".\"",
             strcmp(lc->decimal_point, ".") == 0);
        printf("  INFO: C locale decimal_point = \"%s\"\n", lc->decimal_point);
    }
}

int main(void) {
    printf("=== clib4 Locale Bridge Test Suite ===\n");
    printf("Tests locale.library -> POSIX bridge\n");

    test_setlocale_query();
    test_setlocale_c_locale();
    test_setlocale_system_default();
    test_setlocale_categories();
    test_nl_langinfo_codeset();
    test_nl_langinfo_datetime();
    test_nl_langinfo_day_names();
    test_nl_langinfo_month_names();
    test_nl_langinfo_ampm();
    test_nl_langinfo_yesno();
    test_nl_langinfo_currency();
    test_nl_langinfo_numeric();
    test_nl_langinfo_era();
    test_localeconv();
    test_setlocale_restore();

    printf("\n========================================\n");
    printf("Results: %d passed, %d failed, %d total\n",
           pass_count, fail_count, pass_count + fail_count);
    printf("========================================\n");

    return fail_count > 0 ? 1 : 0;
}
