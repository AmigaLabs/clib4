#define _DEFAULT_SOURCE
#include <stdio.h>
#include <time.h>

int main() {
    struct tm timeinfo = {0};
    timeinfo.tm_year = 2023 - 1900;    // Year - 1900
    timeinfo.tm_mon = 6;               // Month (january = 0)
    timeinfo.tm_mday = 1;              // Day
    timeinfo.tm_hour = 12;             // Hour
    timeinfo.tm_min = 0;               // Minute
    timeinfo.tm_sec = 0;               // Second
    timeinfo.tm_isdst = -1;            // Set if it is DST

    time_t timestamp = timegm(&timeinfo);

    printf("Timestamp UNIX: %ld\n", timestamp);

    return 0;
}





