#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

bool is_prime(int n)
{
    if (n <= 1) return false;
    int i = 2;
    for (; i * i <= n; i++)
        if (n % i == 0)
            return false;
    return true;
}


int primes_count(int a, int b)
{
    int ret = 0;
    for (int i = a; i < b; i++)
        if (is_prime(i) != 0)
            ret++;
    return ret;
}


typedef struct prime_request {
    int a, b;
} prime_request;


void *prime_counter(void *arg)
{
    prime_request req = *(prime_request *)arg;
    int *count = malloc(sizeof(int));
    *count = primes_count(req.a, req.b);
    return (void *)count;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s n m\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]), m = atoi(argv[2]);
    
    if (n < 0 || m < 1) {
        fprintf(stderr, "n >= 0, m >= 1\n");
        return 1;
    }

    pthread_t *threads = malloc((size_t)m * sizeof(pthread_t));
    prime_request *reqs = malloc((size_t)m * sizeof(prime_request));

    int part = n / m;
    for (int k = 0; k < m; k++) {
        reqs[k].a = k * part;
        reqs[k].b = (k == m - 1) ? n : (k + 1) * part;
        pthread_create(&threads[k], NULL, prime_counter, &reqs[k]);
    }

    int total = 0;
    for (int k = 0; k < m; k++) {
        void *res;
        pthread_join(threads[k], &res);
        total += *(int *)res;
        free(res);
    }
    printf("%d\n", total);

    free(threads);
    free(reqs);
    
    return 0;
}
