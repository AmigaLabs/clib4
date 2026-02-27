#include <pthread.h>
#include <stdio.h>
// a simple pthread example
// compile with -lpthreads

#define NUM_THREADS 30

// create the function to be executed as a thread
void *thread(void *ptr) {
    int type = (int) ptr;
    printf("Thread - %d\n", type);
    return ptr;
}

int main(int argc, char **argv) {
    // create the thread objs
    pthread_t threads[NUM_THREADS];
    // start the threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, *thread, (void *) i);
    }

    // wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
