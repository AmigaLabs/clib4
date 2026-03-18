#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_KEYS        32
#define NUM_THREADS     16
#define NUM_ITER        200000
#define THREAD_WAVES    5

typedef struct
{
    int thread_id;
    int key_id;
    long counter;
} tls_data_t;

pthread_key_t keys[NUM_KEYS];

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

void destructor(void *ptr)
{
    tls_data_t *data = (tls_data_t *)ptr;

    if (!data)
        return;

    pthread_mutex_lock(&print_lock);
    printf("Destructor: thread=%d key=%d counter=%ld\n",
           data->thread_id, data->key_id, data->counter);
    pthread_mutex_unlock(&print_lock);

    free(data);
}

void *worker(void *arg)
{
    int tid = (int)(long)arg;

    srand(time(NULL) ^ tid);

    /* inizializza TLS per tutte le keys */
    for (int k = 0; k < NUM_KEYS; k++)
    {
        tls_data_t *data = malloc(sizeof(tls_data_t));

        data->thread_id = tid;
        data->key_id = k;
        data->counter = 0;

        if (pthread_setspecific(keys[k], data) != 0)
        {
            printf("setspecific failed\n");
            exit(1);
        }
    }

    for (long i = 0; i < NUM_ITER; i++)
    {
        int k = rand() % NUM_KEYS;

        tls_data_t *data = pthread_getspecific(keys[k]);

        if (!data)
        {
            printf("Thread %d: NULL TLS on key %d\n", tid, k);
            exit(1);
        }

        if (data->thread_id != tid || data->key_id != k)
        {
            printf("TLS CORRUPTION!\n");
            printf("Expected thread=%d key=%d\n", tid, k);
            printf("Got thread=%d key=%d\n", data->thread_id, data->key_id);
            exit(1);
        }

        data->counter++;

        /* ogni tanto cambia valore */
        if ((i % 50000) == 0)
        {
            tls_data_t *newdata = malloc(sizeof(tls_data_t));
            newdata->thread_id = tid;
            newdata->key_id = k;
            newdata->counter = data->counter;

            pthread_setspecific(keys[k], newdata);

            free(data);
        }

        if ((i % 20000) == 0)
            sched_yield();
    }

    return NULL;
}

int main()
{
    pthread_t threads[NUM_THREADS];

    printf("Creating %d TLS keys\n", NUM_KEYS);

    for (int i = 0; i < NUM_KEYS; i++)
    {
        if (pthread_key_create(&keys[i], destructor) != 0)
        {
            printf("pthread_key_create failed\n");
            return 1;
        }
    }

    for (int wave = 0; wave < THREAD_WAVES; wave++)
    {
        printf("\n--- THREAD WAVE %d ---\n", wave);

        for (int i = 0; i < NUM_THREADS; i++)
        {
            if (pthread_create(&threads[i], NULL, worker, (void *)(long)i) != 0)
            {
                printf("pthread_create failed\n");
                return 1;
            }
        }

        for (int i = 0; i < NUM_THREADS; i++)
            pthread_join(threads[i], NULL);
    }

    printf("\nDeleting keys\n");

    for (int i = 0; i < NUM_KEYS; i++)
        pthread_key_delete(keys[i]);

    printf("Test completed successfully\n");

    return 0;
}