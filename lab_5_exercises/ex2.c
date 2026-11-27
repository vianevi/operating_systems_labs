#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct Thread {
    pthread_t id;
    int i;
    char message[256];
};


static pthread_mutex_t order_lock = PTHREAD_MUTEX_INITIALIZER;

static void *start_routine(void *arg)
{
    struct Thread *t = (struct Thread *)arg;
    pthread_mutex_lock(&order_lock);
    pthread_mutex_unlock(&order_lock);
    printf("Thread %d (id %lu): %s\n", t->i, (unsigned long)t->id, t->message);
    printf("Thread %d exits\n", t->i);
    return NULL;
}

int main(int argc, char *argv[])
{
    int n = (argc > 1) ? atoi(argv[1]) : 5;
    struct Thread *threads = malloc((size_t)n * sizeof(struct Thread));
    if (threads == NULL) return 1;

    for (int i = 0; i < n; i++) {
        threads[i].i = i;
        snprintf(threads[i].message, sizeof threads[i].message, "Hello from thread %d", i);
        pthread_mutex_lock(&order_lock);
        pthread_create(&threads[i].id, NULL, start_routine, &threads[i]);
        printf("Thread %d is created\n", i);
        pthread_mutex_unlock(&order_lock);
        pthread_join(threads[i].id, NULL);
    }
    free(threads);
    return 0;
}
