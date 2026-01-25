#include <stdio.h>
#include <time.h>
#include <stdint.h>

// Replica della funzione timespec_sub per il test
static void timespec_sub(struct timespec *ts1, const struct timespec *ts2, const struct timespec *ts3) {
    ts1->tv_sec = ts2->tv_sec - ts3->tv_sec;
    if (ts2->tv_nsec < ts3->tv_nsec) {
        ts1->tv_sec--;
        ts1->tv_nsec = 1000000000L + ts2->tv_nsec - ts3->tv_nsec;
    } else {
        ts1->tv_nsec = ts2->tv_nsec - ts3->tv_nsec;
    }
}

int main() {
    struct timespec ts1, ts2, ts3, result;

    printf("Test 1: Simple subtraction (no borrow)\n");
    ts2.tv_sec = 10;
    ts2.tv_nsec = 500000000;  // 0.5 seconds
    ts3.tv_sec = 5;
    ts3.tv_nsec = 200000000;  // 0.2 seconds
    timespec_sub(&result, &ts2, &ts3);
    printf("  %ld.%09ld - %ld.%09ld = %ld.%09ld\n",
           ts2.tv_sec, ts2.tv_nsec,
           ts3.tv_sec, ts3.tv_nsec,
           result.tv_sec, result.tv_nsec);
    printf("  Expected: 5.300000000, Got: %ld.%09ld\n", result.tv_sec, result.tv_nsec);
    if (result.tv_sec == 5 && result.tv_nsec == 300000000) {
        printf("  + PASS\n\n");
    } else {
        printf("  x FAIL\n\n");
        return 1;
    }

    printf("Test 2: Subtraction with borrow\n");
    ts2.tv_sec = 10;
    ts2.tv_nsec = 200000000;  // 0.2 seconds
    ts3.tv_sec = 5;
    ts3.tv_nsec = 500000000;  // 0.5 seconds
    timespec_sub(&result, &ts2, &ts3);
    printf("  %ld.%09ld - %ld.%09ld = %ld.%09ld\n",
           ts2.tv_sec, ts2.tv_nsec,
           ts3.tv_sec, ts3.tv_nsec,
           result.tv_sec, result.tv_nsec);
    printf("  Expected: 4.700000000, Got: %ld.%09ld\n", result.tv_sec, result.tv_nsec);
    if (result.tv_sec == 4 && result.tv_nsec == 700000000) {
        printf("  + PASS\n\n");
    } else {
        printf("  x FAIL\n\n");
        return 1;
    }

    printf("Test 3: Subtraction with current time (simulating pthread_cond_timedwait)\n");
    struct timespec current, future;
    clock_gettime(CLOCK_REALTIME, &current);
    future.tv_sec = current.tv_sec + 1;  // 1 second in the future
    future.tv_nsec = current.tv_nsec;

    timespec_sub(&result, &future, &current);
    printf("  Future: %ld.%09ld\n", future.tv_sec, future.tv_nsec);
    printf("  Current: %ld.%09ld\n", current.tv_sec, current.tv_nsec);
    printf("  Difference: %ld.%09ld\n", result.tv_sec, result.tv_nsec);
    printf("  Expected: ~1.000000000, Got: %ld.%09ld\n", result.tv_sec, result.tv_nsec);

    if (result.tv_sec == 1 && result.tv_nsec >= 0 && result.tv_nsec < 1000000) {
        printf("  + PASS\n\n");
    } else if (result.tv_sec == 0 && result.tv_nsec >= 999000000) {
        printf("  + PASS (with slight borrow)\n\n");
    } else {
        printf("  x FAIL\n\n");
        return 1;
    }

    printf("Test 4: Check conversion to TimeVal (uint32)\n");
    ts1.tv_sec = 5;
    ts1.tv_nsec = 500000000;
    uint32_t seconds = (uint32_t)ts1.tv_sec;
    uint32_t microseconds = (uint32_t)(ts1.tv_nsec / 1000);
    printf("  timespec: %ld.%09ld\n", ts1.tv_sec, ts1.tv_nsec);
    printf("  TimeVal: Seconds=%u, Microseconds=%u\n", seconds, microseconds);
    printf("  Expected: Seconds=5, Microseconds=500000\n");
    if (seconds == 5 && microseconds == 500000) {
        printf("  + PASS\n\n");
    } else {
        printf("  x FAIL\n\n");
        return 1;
    }

    printf("Test 5: Check negative tv_nsec doesn't happen\n");
    ts2.tv_sec = 10;
    ts2.tv_nsec = 100000000;  // 0.1 seconds
    ts3.tv_sec = 5;
    ts3.tv_nsec = 900000000;  // 0.9 seconds
    timespec_sub(&result, &ts2, &ts3);
    printf("  %ld.%09ld - %ld.%09ld = %ld.%09ld\n",
           ts2.tv_sec, ts2.tv_nsec,
           ts3.tv_sec, ts3.tv_nsec,
           result.tv_sec, result.tv_nsec);
    printf("  Expected: 4.200000000, Got: %ld.%09ld\n", result.tv_sec, result.tv_nsec);

    // Check that tv_nsec is in valid range [0, 999999999]
    if (result.tv_nsec < 0 || result.tv_nsec >= 1000000000) {
        printf("  x FAIL: tv_nsec out of range!\n\n");
        return 1;
    }

    if (result.tv_sec == 4 && result.tv_nsec == 200000000) {
        printf("  + PASS\n\n");
    } else {
        printf("  x FAIL\n\n");
        return 1;
    }

    printf("All tests passed! +\n");
    return 0;
}
