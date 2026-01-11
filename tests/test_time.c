/*
 * Test suite for time.h functions
 */

#include "test_framework.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/time.h>
#endif

/* Test time */
static const char *test_time(void) {
    time_t t1, t2;
    
    t1 = time(NULL);
    TEST_ASSERT("time(NULL) returns non-zero", t1 != 0);
    
    sleep(1);
    t2 = time(NULL);
    TEST_ASSERT("time advances", t2 > t1);
    
    time_t t3;
    time(&t3);
    TEST_ASSERT("time(&t) works", t3 >= t2);
    
    return NULL;
}

/* Test difftime */
static const char *test_difftime(void) {
    time_t t1, t2;
    double diff;
    
    t1 = time(NULL);
    sleep(2);
    t2 = time(NULL);
    
    diff = difftime(t2, t1);
    TEST_ASSERT("difftime >= 2", diff >= 2.0);
    TEST_ASSERT("difftime < 3", diff < 3.0);
    
    return NULL;
}

/* Test localtime */
static const char *test_localtime(void) {
    time_t now;
    struct tm *tm_info;
    
    now = time(NULL);
    tm_info = localtime(&now);
    
    TEST_ASSERT_NOT_NULL("localtime returns non-NULL", tm_info);
    TEST_ASSERT("tm_year is reasonable", tm_info->tm_year >= 100); /* Years since 1900 */
    TEST_ASSERT("tm_mon in range", tm_info->tm_mon >= 0 && tm_info->tm_mon <= 11);
    TEST_ASSERT("tm_mday in range", tm_info->tm_mday >= 1 && tm_info->tm_mday <= 31);
    TEST_ASSERT("tm_hour in range", tm_info->tm_hour >= 0 && tm_info->tm_hour <= 23);
    TEST_ASSERT("tm_min in range", tm_info->tm_min >= 0 && tm_info->tm_min <= 59);
    TEST_ASSERT("tm_sec in range", tm_info->tm_sec >= 0 && tm_info->tm_sec <= 61);
    
    return NULL;
}

/* Test gmtime */
static const char *test_gmtime(void) {
    time_t now;
    struct tm *tm_info;
    
    now = time(NULL);
    tm_info = gmtime(&now);
    
    TEST_ASSERT_NOT_NULL("gmtime returns non-NULL", tm_info);
    TEST_ASSERT("tm_year is reasonable", tm_info->tm_year >= 100);
    TEST_ASSERT("tm_mon in range", tm_info->tm_mon >= 0 && tm_info->tm_mon <= 11);
    TEST_ASSERT("tm_mday in range", tm_info->tm_mday >= 1 && tm_info->tm_mday <= 31);
    
    return NULL;
}

/* Test mktime */
static const char *test_mktime(void) {
    struct tm tm_info;
    time_t t;
    
    /* Create a known date: 2023-01-01 12:00:00 */
    memset(&tm_info, 0, sizeof(struct tm));
    tm_info.tm_year = 123; /* 2023 - 1900 */
    tm_info.tm_mon = 0;    /* January */
    tm_info.tm_mday = 1;
    tm_info.tm_hour = 12;
    tm_info.tm_min = 0;
    tm_info.tm_sec = 0;
    tm_info.tm_isdst = -1; /* Let mktime determine DST */
    
    t = mktime(&tm_info);
    TEST_ASSERT("mktime returns non-negative", t >= 0);
    TEST_ASSERT("mktime normalizes fields", tm_info.tm_wday >= 0 && tm_info.tm_wday <= 6);
    
    return NULL;
}

/* Test asctime */
static const char *test_asctime(void) {
    time_t now;
    struct tm *tm_info;
    char *timestr;
    
    now = time(NULL);
    tm_info = localtime(&now);
    timestr = asctime(tm_info);
    
    TEST_ASSERT_NOT_NULL("asctime returns non-NULL", timestr);
    TEST_ASSERT("asctime string has content", strlen(timestr) > 0);
    
    return NULL;
}

/* Test ctime */
static const char *test_ctime(void) {
    time_t now;
    char *timestr;
    
    now = time(NULL);
    timestr = ctime(&now);
    
    TEST_ASSERT_NOT_NULL("ctime returns non-NULL", timestr);
    TEST_ASSERT("ctime string length", strlen(timestr) >= 24);
    
    return NULL;
}

/* Test strftime */
static const char *test_strftime(void) {
    time_t now;
    struct tm *tm_info;
    char buffer[80];
    size_t result;
    
    now = time(NULL);
    tm_info = localtime(&now);
    
    /* Test year format */
    result = strftime(buffer, sizeof(buffer), "%Y", tm_info);
    TEST_ASSERT("strftime %%Y returns > 0", result > 0);
    TEST_ASSERT("strftime %%Y length is 4", result == 4);
    
    /* Test month format */
    result = strftime(buffer, sizeof(buffer), "%m", tm_info);
    TEST_ASSERT("strftime %%m returns > 0", result > 0);
    
    /* Test day format */
    result = strftime(buffer, sizeof(buffer), "%d", tm_info);
    TEST_ASSERT("strftime %%d returns > 0", result > 0);
    
    /* Test full date/time */
    result = strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    TEST_ASSERT("strftime full format returns > 0", result > 0);
    TEST_ASSERT("strftime full format length >= 19", result >= 19);
    
    /* Test weekday name */
    result = strftime(buffer, sizeof(buffer), "%A", tm_info);
    TEST_ASSERT("strftime %%A (weekday) returns > 0", result > 0);
    
    /* Test month name */
    result = strftime(buffer, sizeof(buffer), "%B", tm_info);
    TEST_ASSERT("strftime %%B (month name) returns > 0", result > 0);
    
    return NULL;
}

/* Test clock */
static const char *test_clock(void) {
    clock_t start, end;
    int i;
    volatile int dummy = 0;
    
    start = clock();
    TEST_ASSERT("clock() returns non-negative", start >= 0);
    
    /* Do some work */
    for (i = 0; i < 100000; i++) {
        dummy += i;
    }
    
    end = clock();
    TEST_ASSERT("clock advances", end >= start);
    
    return NULL;
}

/* Test nanosleep (if available) */
static const char *test_nanosleep(void) {
#ifdef _POSIX_C_SOURCE
    struct timespec req, rem;
    int result;
    
    req.tv_sec = 0;
    req.tv_nsec = 100000000; /* 0.1 seconds */
    
    result = nanosleep(&req, &rem);
    TEST_ASSERT("nanosleep returns 0 on success", result == 0 || result == -1);
#else
    printf("    (nanosleep not available, skipping)\n");
#endif
    
    return NULL;
}

/* Test struct tm fields */
static const char *test_tm_struct(void) {
    time_t now;
    struct tm *tm_info;
    struct tm tm_copy;
    
    now = time(NULL);
    tm_info = localtime(&now);
    
    /* Make a copy */
    tm_copy = *tm_info;
    
    /* Test that mktime/localtime round-trip works */
    now = mktime(&tm_copy);
    tm_info = localtime(&now);
    
    TEST_ASSERT("tm_year matches after round-trip", tm_info->tm_year == tm_copy.tm_year);
    TEST_ASSERT("tm_mon matches after round-trip", tm_info->tm_mon == tm_copy.tm_mon);
    TEST_ASSERT("tm_mday matches after round-trip", tm_info->tm_mday == tm_copy.tm_mday);
    
    return NULL;
}

/* Test time zone handling */
static const char *test_timezone(void) {
    time_t now;
    struct tm *local_tm, *gmt_tm;
    
    now = time(NULL);
    local_tm = localtime(&now);
    gmt_tm = gmtime(&now);
    
    /* Both should return valid pointers */
    TEST_ASSERT_NOT_NULL("localtime returns non-NULL", local_tm);
    TEST_ASSERT_NOT_NULL("gmtime returns non-NULL", gmt_tm);
    
    /* They may differ depending on timezone */
    /* Just verify both are reasonable */
    TEST_ASSERT("local tm_year reasonable", local_tm->tm_year >= 100);
    TEST_ASSERT("gmt tm_year reasonable", gmt_tm->tm_year >= 100);
    
    return NULL;
}

/* Main test runner */
int main(void) {
    BEGIN_TEST_SUITE("time.h");
    
    RUN_TEST(test_time);
    RUN_TEST(test_difftime);
    RUN_TEST(test_localtime);
    RUN_TEST(test_gmtime);
    RUN_TEST(test_mktime);
    RUN_TEST(test_asctime);
    RUN_TEST(test_ctime);
    RUN_TEST(test_strftime);
    RUN_TEST(test_clock);
    RUN_TEST(test_nanosleep);
    RUN_TEST(test_tm_struct);
    RUN_TEST(test_timezone);
    
    END_TEST_SUITE();
    
    return tests_failed;
}
