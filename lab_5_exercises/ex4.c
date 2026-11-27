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


pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;


int k = 0;
int c = 0;
int n = 0;


int get_number_to_check()
{
    int ret = k;
    if (k != n)
        k++;
    return ret;
}


void increment_primes()
{
    c++;
}

void *check_primes(void *arg)
{
    (void)arg;
    while (1) {
        pthread_mutex_lock(&global_lock);
        int num = get_number_to_check();
        pthread_mutex_unlock(&global_lock);

        if (num == n)
            break;

        if (is_prime(num)) {
            pthread_mutex_lock(&global_lock);
            increment_primes();
            pthread_mutex_unlock(&global_lock);
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s n m\n", argv[0]);
        return 1;
    }
    n = atoi(argv[1]);
    int m = atoi(argv[2]);

    if (n < 0 || m < 1) {
        fprintf(stderr, "n >= 0, m >= 1\n");
        return 1;
    }

    pthread_t *threads = malloc((size_t)m * sizeof(pthread_t));

    for (int i = 0; i < m; i++)
        pthread_create(&threads[i], NULL, check_primes, NULL);

    for (int i = 0; i < m; i++)
        pthread_join(threads[i], NULL);

    printf("%d\n", c);
    free(threads);
    pthread_mutex_destroy(&global_lock);
    
    return 0;
}
