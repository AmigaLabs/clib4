#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define N 30
pthread_key_t key;
pthread_once_t once = PTHREAD_ONCE_INIT;

void key_create() {
    pthread_key_create(&key, NULL);
}

void *strerror1(void *ptr) {

    int *n;
    n = (int *) ptr;
    char *str;

    pthread_once(&once, key_create);
    str = pthread_getspecific(key);

    if (str == NULL) {
        str = malloc(sizeof(char) * 20);
        pthread_setspecific(key, str);
    }
    strcpy(str, strerror(*n));                                                  //unique adress
    printf("[%d] %s\n", *n, str);

    return NULL;
}

int main() {

    int i = 0;
    int err[N];
    pthread_t thread[N];

    for (i = 0; i < N; i++) {
        err[i] = i;                                                        //unique adress
        pthread_create(&thread[i], NULL, strerror1, (void *) &err[i]);
    }

    for (i = 0; i < N; i++) {
        pthread_join(thread[i], NULL);
    }

    return 0;
}