#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 8
#define NUM_CHILD_THREADS 4
#define ITERATIONS 100000

pthread_key_t tls_key;

void tls_destructor(void *value)
{
    printf("Destructor called for thread %lu value=%p\n",
           (unsigned long)pthread_self(), value);
    free(value);
}

void *child_thread(void *arg)
{
    long id = (long)arg;

    void *v = pthread_getspecific(tls_key);

    if (v != NULL)
    {
        printf("ERROR: child thread inherited TLS! thread=%lu value=%p\n",
               (unsigned long)pthread_self(), v);
    }

    int *data = malloc(sizeof(int));
    *data = id * 1000;

    pthread_setspecific(tls_key, data);

    for (int i = 0; i < ITERATIONS; i++)
    {
        int *check = pthread_getspecific(tls_key);

        if (!check || *check != id * 1000)
        {
            printf("TLS CORRUPTION in child thread %lu\n",
                   (unsigned long)pthread_self());
            exit(1);
        }
    }

    return NULL;
}

void *thread_func(void *arg)
{
    long id = (long)arg;

    int *data = malloc(sizeof(int));
    *data = id;

    pthread_setspecific(tls_key, data);

    for (int i = 0; i < ITERATIONS; i++)
    {
        int *check = pthread_getspecific(tls_key);

        if (!check || *check != id)
        {
            printf("TLS CORRUPTION in thread %ld (%lu)\n",
                   id, (unsigned long)pthread_self());
            exit(1);
        }
    }

    pthread_t children[NUM_CHILD_THREADS];

    for (long i = 0; i < NUM_CHILD_THREADS; i++)
    {
        pthread_create(&children[i], NULL, child_thread, (void *)id);
    }

    for (int i = 0; i < NUM_CHILD_THREADS; i++)
    {
        pthread_join(children[i], NULL);
    }

    int *check = pthread_getspecific(tls_key);

    if (!check || *check != id)
    {
        printf("TLS LOST after children in thread %ld\n", id);
        exit(1);
    }

    return NULL;
}

int main()
{
    pthread_key_create(&tls_key, tls_destructor);

    pthread_t threads[NUM_THREADS];

    for (long i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, thread_func, (void *)i);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_key_delete(tls_key);

    printf("\nTEST COMPLETED SUCCESSFULLY\n");

    return 0;
}