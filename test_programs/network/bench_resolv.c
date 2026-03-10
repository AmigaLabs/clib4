#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <resolv.h>
#include <arpa/nameser.h>

#define DEFAULT_THREADS 4
#define DEFAULT_ITER    50
#define ANSWER_SIZE 4096

typedef struct {
    const char *domain;
    int iterations;
    int qtype;
} thread_arg;

/* ---------------- timing ---------------- */

static double now_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

/* ---------------- random subdomain (cache bypass) ---------------- */

static void random_domain(char *out, size_t size, const char *base)
{
    unsigned r = rand();
    snprintf(out, size, "bench-%u.%s", r, base);
}

/* ---------------- DNS parsing ---------------- */

static void parse_dns(unsigned char *answer, int len)
{
    ns_msg msg;
    if (ns_initparse(answer, len, &msg) < 0)
        return;

    int count = ns_msg_count(msg, ns_s_an);

    for (int i = 0; i < count; i++)
    {
        ns_rr rr;
        if (ns_parserr(&msg, ns_s_an, i, &rr) < 0)
            continue;

        const u_char *rdata = ns_rr_rdata(rr);

        switch (ns_rr_type(rr))
        {
            case ns_t_a:
            case ns_t_aaaa:
                /* solo verifica presenza */
                break;

            case ns_t_mx:
                /* MX priority + name */
                break;

            case ns_t_txt:
                /* TXT data */
                break;
        }
    }
}

/* ---------------- worker thread ---------------- */

void *worker(void *argptr)
{
    thread_arg *arg = argptr;
    unsigned char answer[ANSWER_SIZE];

    double total = 0, min = 1e18, max = 0;

    char domainbuf[512];

    for (int i = 0; i < arg->iterations; i++)
    {
        random_domain(domainbuf, sizeof(domainbuf), arg->domain);

        double start = now_ns();

        int len = res_query(domainbuf, ns_c_in, arg->qtype,
                            answer, sizeof(answer));

        double end = now_ns();
        double elapsed = end - start;

        if (len > 0)
            parse_dns(answer, len);

        total += elapsed;
        if (elapsed < min) min = elapsed;
        if (elapsed > max) max = elapsed;
    }

    printf("Thread %lu type %d\n", pthread_self(), arg->qtype);
    printf(" Avg %.3f ms | Min %.3f | Max %.3f\n",
           (total/arg->iterations)/1e6, min/1e6, max/1e6);

    return NULL;
}

/* ---------------- main ---------------- */

int main(int argc, char **argv)
{
    const char *domain = "google.com";
    int threads = DEFAULT_THREADS;
    int iterations = DEFAULT_ITER;

    if (argc > 1) domain = argv[1];
    if (argc > 2) threads = atoi(argv[2]);
    if (argc > 3) iterations = atoi(argv[3]);

    srand(time(NULL));
    res_init();

    int qtypes[] = { ns_t_a, ns_t_aaaa, ns_t_mx, ns_t_txt };
    int qcount = sizeof(qtypes)/sizeof(qtypes[0]);

    pthread_t *t = malloc(sizeof(pthread_t) * threads * qcount);
    thread_arg *args = malloc(sizeof(thread_arg) * threads * qcount);

    int idx = 0;

    for (int q = 0; q < qcount; q++)
    {
        for (int i = 0; i < threads; i++)
        {
            args[idx].domain = domain;
            args[idx].iterations = iterations;
            args[idx].qtype = qtypes[q];

            pthread_create(&t[idx], NULL, worker, &args[idx]);
            idx++;
        }
    }

    for (int i = 0; i < idx; i++)
        pthread_join(t[i], NULL);

    free(t);
    free(args);

    return 0;
}
