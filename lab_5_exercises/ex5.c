#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define QSIZE 128
#define DONE -1

static int queue[QSIZE], head = 0, tail = 0, count = 0;

static pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

static int primes = 0;
static pthread_mutex_t plock = PTHREAD_MUTEX_INITIALIZER;
static int n, m;

static bool is_prime_6k(int x)
{
    if (x == 2 || x == 3) {
        return true;
    }

    if (x < 5){
        return false;
    }

    for (int i = 5; (long)i * i <= x; i += 6)
        if (x % i == 0 || x % (i + 2) == 0) {
            return false;
        }

    return true;
}

static void produce(int v)
{
    pthread_mutex_lock(&qlock);
    while (count == QSIZE)
        pthread_cond_wait(&not_full, &qlock);
    queue[tail] = v; tail = (tail + 1) % QSIZE; count++;

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&qlock);
}

static int consume(void)
{
    pthread_mutex_lock(&qlock);
    while (count == 0)
        pthread_cond_wait(&not_empty, &qlock);

    int v = queue[head]; head = (head + 1) % QSIZE; count--;
    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&qlock);
    return v;
}

static void *producer(void *arg)
{
    (void)arg;
    for (int x = 2; x < n; x++)
        if (x == 2 || x == 3 || (x % 2 != 0 && x % 3 != 0))
            produce(x);
    for (int i = 0; i < m; i++) {
        produce(DONE);
    }
    return NULL;
}

static void *consumer(void *arg)
{
    (void)arg;
    for (int v; (v = consume()) != DONE; )
        if (is_prime_6k(v)) {
            pthread_mutex_lock(&plock);
            primes++;
            pthread_mutex_unlock(&plock);
        }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s n m\n", argv[0]);
        return 1;
    
    }
    n = atoi(argv[1]); m = atoi(argv[2]);
    pthread_t prod, *cons = malloc((size_t)m * sizeof(pthread_t));
    pthread_create(&prod, NULL, producer, NULL);
    for (int i = 0; i < m; i++) {
        pthread_create(&cons[i], NULL, consumer, NULL);
    }

    pthread_join(prod, NULL);
    for (int i = 0; i < m; i++) {
        pthread_join(cons[i], NULL);
    }

    printf("%d\n", primes);
    free(cons);
    
    return 0;
}
