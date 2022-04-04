#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
//#include <sys/_structs.h>

using namespace std;

#define NUM_THREADS 10
#define DIGITS 4

void *thread_function(void *);

int main(int argc, char *argv[])
{
    time_t start1, start2, start3;
    time_t end1, end2, end3;
    tms start_buffer, finish_buffer;
    clock_t start, finish;
    int iterations, i, rc;
    void *status;
    double duration;
    rusage usage_start, usage_end;
    timespec start_time, end_time;
    struct timeval myTV1, myTV2;

    if (argc != 2)
    {
        cout << "Usage error. I am expecting one positive integer." << endl;
        exit(1);
    }
    iterations = atoi(argv[1]);

    // create opaque objects.
    pthread_t *threads = new pthread_t[NUM_THREADS];
    // Make threads Joinable for sure.
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    start1 = clock();
    start2 = time(NULL);
    time(&start3);
    int const ticks_per_sec = sysconf(_SC_CLK_TCK);
    cout << "Clock ticks per second: " << ticks_per_sec << endl;
    start = times(&start_buffer);
    if (getrusage(RUSAGE_SELF, &usage_start) != 0)
        cerr << " getrusage returned error!" << endl;
#ifndef NEWLIB
    clock_gettime (CLOCK_REALTIME, &start_time);
#endif
    gettimeofday(&myTV1, NULL);

    cout << "Just started counting!" << std::endl;

    for (i = 0; i < NUM_THREADS; i++)
    {
        rc = pthread_create(&(threads[i]), &attr, thread_function, (void *)&iterations);
        if (rc)
        {
            cerr << "ERROR; return code from pthread_create() is " << rc << std::endl;
            exit(2);
        }
    }

    for (i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], &status);
    cout << "All threads joined!" << std::endl;

    end1 = clock();
    end2 = time(NULL);
    time(&end3);
    finish = times(&finish_buffer);
    if (getrusage(RUSAGE_SELF, &usage_end) != 0)
        cerr << " getrusage returned error! [2]" << endl;
    //clock_gettime (CLOCK_REALTIME, end_time);
    gettimeofday(&myTV2, NULL);

    duration = static_cast<double>(end1 - start1);
    if (duration < static_cast<double>(CLOCKS_PER_SEC))
    {
        cout << "Few iterations. Try again with a bigger number." << endl;
        exit(3);
    }

    cout << "Duration 1: " << fixed << setprecision(DIGITS) << static_cast<double>(duration / CLOCKS_PER_SEC) << " secs." << endl;
    cout << "Duration 2: " << endl;
    cout << "* started : " << start2 << " seconds after 1/1/1970." << endl;
    cout << "* finished: " << end2 << " seconds after 1/1/1970." << endl;
    cout << "Duration 3: " << static_cast<int>(difftime(end3, start3)) << " secs." << endl;
    cout << "Duration 4: " << static_cast<double>((finish - start) / ticks_per_sec) << "." << endl;
    cout << "tms_utime for start  : " << start_buffer.tms_utime << "." << endl;
    cout << "tms_utime for finish : " << finish_buffer.tms_utime << "." << endl;
    cout << "tms_stime for start  : " << start_buffer.tms_stime << "." << endl;
    cout << "tms_stime for finish : " << finish_buffer.tms_stime << "." << endl;
    cout << "tms_cutime for start : " << start_buffer.tms_cutime << "." << endl;
    cout << "tms_cutime for finish: " << finish_buffer.tms_cutime << "." << endl;
    cout << "tms_cstime for start : " << start_buffer.tms_cstime << "." << endl;
    cout << "tms_cstime for finish: " << finish_buffer.tms_cstime << "." << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "rusage: ru_utime start: " << usage_start.ru_utime.tv_sec << "." << endl;
    cout << "rusage: ru_utime start: " << usage_start.ru_utime.tv_usec << "." << endl;
    cout << "rusage: ru_stime start: " << usage_start.ru_stime.tv_sec << "." << endl;
    cout << "rusage: ru_stime start: " << usage_start.ru_stime.tv_usec << "." << endl;
    cout << "rusage: ru_utime end  : " << usage_end.ru_utime.tv_sec << "." << endl;
    cout << "rusage: ru_utime end  : " << usage_end.ru_utime.tv_usec << "." << endl;
    cout << "rusage: ru_stime end  : " << usage_end.ru_stime.tv_sec << "." << endl;
    cout << "rusage: ru_stime end  : " << usage_end.ru_stime.tv_usec << "." << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "gettime: tv_sec start : " << start_time.tv_sec << "." << endl;
    cout << "gettime: tv_nsec start: " << start_time.tv_nsec << "." << endl;
    cout << "gettime: tv_sec end   : " << end_time.tv_sec << "." << endl;
    cout << "gettime: tv_nsec end  : " << end_time.tv_nsec << "." << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "timeofday - start sec : " << myTV1.tv_sec << "." << endl;
    cout << "timeofday - start usec: " << myTV1.tv_usec << "." << endl;
    cout << "timeofday - end sec   : " << myTV2.tv_sec << "." << endl;
    cout << "timeofday - end usec  : " << myTV2.tv_usec << "." << endl;

    return 0;
}

void *thread_function(void *input)
{
    int loops = *((int *)input);
    int i;

    for (i = 0; i < loops; i++)
        rand() % RAND_MAX; // Just a dummy operation.

    pthread_exit(NULL);
}