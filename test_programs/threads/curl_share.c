#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <curl/curl.h>
#include <unistd.h>

#define NUM_THREADS 10
#define REQUESTS_PER_THREAD 20

// Compile with: ppc-amigaos-gcc -pthread -mcrt=clib4 curl_share.c -o curl_share -lcurl -lrtmp -lssl -lcrypto -lz

CURLSH *share;

/* ===== LOCK CALLBACKS (QUI vengono usati i mutex interni di curl) ===== */

void lock_cb(CURL *handle, curl_lock_data data, curl_lock_access access, void *userptr) {
    const char *what;
    switch(data) {
        case CURL_LOCK_DATA_DNS: what = "DNS"; break;
        case CURL_LOCK_DATA_SSL_SESSION: what = "SSL_SESSION"; break;
        case CURL_LOCK_DATA_CONNECT: what = "CONNECT"; break;
        default: what = "OTHER"; break;
    }
    printf("+ Thread %lu locking %s\n", pthread_self(), what);
}

void unlock_cb(CURL *handle, curl_lock_data data, void *userptr) {
    const char *what;
    switch(data) {
        case CURL_LOCK_DATA_DNS: what = "DNS"; break;
        case CURL_LOCK_DATA_SSL_SESSION: what = "SSL_SESSION"; break;
        case CURL_LOCK_DATA_CONNECT: what = "CONNECT"; break;
        default: what = "OTHER"; break;
    }
    printf("- Thread %lu unlocking %s\n", pthread_self(), what);
}

/* ===== THREAD WORKER ===== */

void *worker(void *arg) {
    long id = (long)arg;

    for(int i = 1; i <= REQUESTS_PER_THREAD; i++) {
        printf("Thread %ld starting request %d\n", id, i);
        CURL *curl = curl_easy_init();
        if(!curl) 
            continue;
        printf("Thread %ld initialized CURL handle\n", id);
        curl_easy_setopt(curl, CURLOPT_URL, "http://google.com/");
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // solo HEAD, più veloce
        //curl_easy_setopt(curl, CURLOPT_SHARE, share);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

        printf("Thread %ld performing request %d\n", id, i);
        CURLcode res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "Thread %ld error: %s\n", id, curl_easy_strerror(res));
        }
        printf("Thread %ld completed request %d\n", id, i);
        curl_easy_cleanup(curl);
        printf("Thread %ld cleaned up CURL handle. Sleeping...\n", id);
        usleep(200000); // 200ms per aumentare interleave tra thread
        printf("Thread %ld woke up from sleep\n", id);
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    curl_global_init(CURL_GLOBAL_ALL);
#if 0
    share = curl_share_init();
    curl_share_setopt(share, CURLSHOPT_LOCKFUNC, lock_cb);
    curl_share_setopt(share, CURLSHOPT_UNLOCKFUNC, unlock_cb);

    /* Queste opzioni fanno usare i mutex interni di curl */
    curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
    curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
    curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
#endif
    for(long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, (void*)i);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        printf("Joining thread %d\n", i);
        pthread_join(threads[i], NULL);
    }
    printf("All threads completed. Share cleanup starting...\n");

    curl_share_cleanup(share);
    printf("Share cleaned up. Global cleanup starting...\n");
    curl_global_cleanup();

    printf("Done.\n");
    return 0;
}
