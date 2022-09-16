#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NB_THREADS 1
#define NB_KEYS 1

static pthread_key_t keys[NB_KEYS];
static pthread_mutex_t mutex;

void destruction(void *arg) {
    (int) arg++;
    printf("Destructor called! -- key value : %i\n", (int) arg);
}

void *startup_routine(void *argv) {
    int i;
    int th = (int) argv;

    for (i = 0; i < NB_KEYS; i++)
        pthread_setspecific(keys[i], (void *) ((th + i) * 2));

    pthread_mutex_lock(&mutex);

    printf("Thread %i\n", th);

    for (i = 0; i < NB_KEYS; i++)
        printf("\tkeys[%i] : %i\n", i, (int) pthread_getspecific(keys[i]));

    pthread_mutex_unlock(&mutex);

    return "End";
}

int main(int argc, char **argv) {
    int i;
    void *result;
    pthread_t thread[NB_THREADS];

    for (i = 0; i < NB_KEYS; i++)
        pthread_key_create(&keys[i], destruction);

    pthread_mutex_init(&mutex, NULL);

    for (i = 0; i < NB_THREADS; i++)
        pthread_create(&thread[i], NULL, startup_routine, (void *) (i + 1));

    for (i = 0; i < NB_THREADS; i++) {
        pthread_join(thread[i], &result);
        printf("Return from the thread %i = '%s'\n", i, (char *) result);
    }

    return 0;
}