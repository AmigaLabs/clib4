#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static void *fct(void *p) {
    int number = *(int *) p;
    int *arr = malloc(number * sizeof(int));  // Error check missing!
    for (int i = 0; i < number; i++)
        arr[i] = number + (7 * i + 5) % 11;
    pthread_mutex_lock(&mtx);   // Error check missing!
    printf("Integers %2d (%p):", number, (void *) arr);
    for (int i = 0; i < number; i++)
        printf(" %3d", arr[i]);
    putchar('\n');
    pthread_mutex_unlock(&mtx); // Error check missing!
    return (void *) arr;
}

enum {
    MAX_THREADS = 10
};

int main(void) {
    pthread_t threads[MAX_THREADS];
    int input[MAX_THREADS];
    int *result[MAX_THREADS];

    /* Initialization */
    for (int i = 0; i < MAX_THREADS; i++)
        input[i] = (3 * i + 2) % 13 + 1;

    /* Thread creation */
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_mutex_lock(&mtx);   // Error check missing!
        printf("Launch %d: %2d\n", i, input[i]);
        pthread_mutex_unlock(&mtx); // Error check missing!
        int rc = pthread_create(&threads[i], NULL, fct, (void *) &input[i]);
        if (rc != 0) {
            fprintf(stderr, "Oops creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    /* Thread harvesting */
    for (int i = MAX_THREADS; i-- > 0;) {
        void *vp;
        int rc = pthread_join(threads[i], &vp);
        if (rc != 0) {
            fprintf(stderr, "Oops joining thread %d\n", i);
            exit(EXIT_FAILURE);
        }
        printf("Thread %d: %2d (%p)\n", i, input[i], vp);
        result[i] = vp;
    }

    /* Finalization */
    for (int i = 0; i < MAX_THREADS; i++) {
        printf("Master - %d:", i);
        for (int j = 0; j < input[i]; j++)
            printf(" %3d", result[i][j]);
        putchar('\n');
        free(result[i]);
    }

    pthread_mutex_destroy(&mtx);
    return 0;
}
