#include <stdio.h>
#include <pthread.h>

// Compile with: ppc-amigaos-gcc -mcrt=clib4 -O3 tls_thread.c -o tls_thread -lpthread -athread=native
__thread int my_tls_variable = 0;

void* thread_function(void* arg) {
    int thread_id = *(int*)arg;
    my_tls_variable = thread_id * 10;
    printf("Thread %d: my_tls_variable = %d\n", thread_id, my_tls_variable);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    my_tls_variable = 100; // Value for main thread
    printf("Main thread: my_tls_variable = %d\n", my_tls_variable);

    pthread_create(&t1, NULL, thread_function, &id1);
    pthread_create(&t2, NULL, thread_function, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Main thread (after other threads): my_tls_variable = %d\n", my_tls_variable);

    return 0;
}
