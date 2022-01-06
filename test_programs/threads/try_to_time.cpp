#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 10
#define DIGITS 4

void *thread_function(void *);

int main(int argc, char *argv[])
{
    time_t start1, start2, start3;
    time_t end1, end2, end3;
    int iterations, i, rc;
    void *status;
    double duration;

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

    duration = static_cast<double>(end1 - start1);
    if (duration < static_cast<double>(CLOCKS_PER_SEC))
    {
        cout << "Few iterations. Try again with a bigger number." << endl;
        exit(3);
    }

    cout << "Duration 1: " << fixed << setprecision(DIGITS)
         << static_cast<double>(duration / CLOCKS_PER_SEC) << " secs." << endl;
    cout << "Duration 2: " << endl;
    cout << "* started : " << start2 << " seconds after 1/1/1970." << endl;
    cout << "* finished: " << end2 << " seconds after 1/1/1970." << endl;
    cout << "Duration 3: " << static_cast<int>(difftime(end3, start3)) << " secs." << endl;

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