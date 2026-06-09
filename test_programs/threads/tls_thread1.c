#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 8
#define NUM_ITER    100000

typedef struct
{
    int thread_id;
    long counter;
} thread_data_t;

pthread_key_t tls_key;

void tls_destructor(void *ptr)
{
    thread_data_t *data = (thread_data_t *)ptr;

    if (data)
    {
        printf("Destructor called for thread %d, final counter=%ld\n",
               data->thread_id, data->counter);
        free(data);
    }
}

void *thread_func(void *arg)
{
    int tid = (int)(long)arg;

    thread_data_t *data = malloc(sizeof(thread_data_t));
    data->thread_id = tid;
    data->counter = 0;

    if (pthread_setspecific(tls_key, data) != 0)
    {
        printf("pthread_setspecific failed\n");
        exit(1);
    }

    for (long i = 0; i < NUM_ITER; i++)
    {
        thread_data_t *mydata = pthread_getspecific(tls_key);

        if (!mydata)
        {
            printf("Thread %d: TLS returned NULL!\n", tid);
            exit(1);
        }

        if (mydata->thread_id != tid)
        {
            printf("Thread %d: TLS corruption detected! got %d\n",
                   tid, mydata->thread_id);
            exit(1);
        }

        mydata->counter++;

        if ((i % 20000) == 0)
            sched_yield();
    }

    thread_data_t *final = pthread_getspecific(tls_key);

    printf("Thread %d finished, counter=%ld\n", tid, final->counter);

    return NULL;
}

int main()
{
    pthread_t threads[NUM_THREADS];

    if (pthread_key_create(&tls_key, tls_destructor) != 0)
    {
        printf("pthread_key_create failed\n");
        return 1;
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads[i], NULL, thread_func, (void *)(long)i) != 0)
        {
            printf("pthread_create failed\n");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    pthread_key_delete(tls_key);

    printf("All threads completed\n");

    return 0;
}