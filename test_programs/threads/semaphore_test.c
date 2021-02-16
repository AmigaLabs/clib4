// C program to demonstrate working of Semaphores - this fail on newlib if you press CTRL-C while should work with clib2
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

void *thread(void *arg);
sem_t mutex;

void *thread(void *arg)
{
    //wait
    sem_wait(&mutex);
    printf("\nEntered..\n");

    //critical section
    sleep(4);

    //signal
    printf("\nJust Exiting...\n");
    sem_post(&mutex);
}

int main()
{
    sem_init(&mutex, 0, 1);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread, NULL);
    sleep(2);
    pthread_create(&t2, NULL, thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    sem_destroy(&mutex);
    return 0;
}
