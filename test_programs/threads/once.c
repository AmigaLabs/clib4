#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// The once control variable must be statically initialized
static pthread_once_t once_control = PTHREAD_ONCE_INIT;

// This will be our initialization function
static void init_function(void)
{
    printf("Initialization function called (only once)\n");
    // Here you would typically do one-time initialization:
    // - Open files
    // - Allocate resources
    // - Initialize global variables
    // - etc.
}

// Thread function that uses the once initialization
void* thread_func(void* arg)
{
    int thread_num = *(int*)arg;
    printf("Thread %d started\n", thread_num);
    
    // All threads call pthread_once, but init_function will execute only once
    pthread_once(&once_control, init_function);
    
    printf("Thread %d continuing with its work\n", thread_num);
    return NULL;
}

int main()
{
    pthread_t threads[5];
    int thread_nums[5];
    
    // Create 5 threads
    for (int i = 0; i < 5; i++) {
        thread_nums[i] = i + 1;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_nums[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("All threads completed\n");
    return 0;
}