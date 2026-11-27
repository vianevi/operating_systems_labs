#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define TRI_BASE 1000000

pid_t pid;
int process_idx;
long tris;

bool is_triangular(int n)
{
    for (long long i = 1; i <= n; i++) {
        if (i * (i + 1) == 2LL * n) {
            return true;
        }
    }
    return false;
}


long big_n()
{
    time_t t;
    long n = 0;
    srand((unsigned)time(&t));

    while (n < TRI_BASE) {
        n += rand();
    }

    return n % TRI_BASE;
}


static void say(const char *text)
{
    write(STDOUT_FILENO, text, strlen(text));
}

void signal_handler(int signum)
{
    char buf[200];
    switch (signum) {
    case SIGTSTP:
        snprintf(buf, sizeof buf, "Process %d: has stopped\n", process_idx, pid);
        say(buf);
        raise(SIGSTOP);
        break;
    case SIGCONT:
        snprintf(buf, sizeof buf, "Process %d: has been continued\n", process_idx, pid);
        say(buf);
        break;
    case SIGTERM:
        snprintf(buf, sizeof buf, "Process %d: count of triangulars found so far is %ld\n", process_idx, pid, tris);
        say(buf);
        snprintf(buf, sizeof buf, "Process %d: terminating \n", process_idx);
        say(buf);

        _exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s idx\n", argv[0]);
        return 1;
    
    }
    process_idx = atoi(argv[1]);
    pid = getpid();
    setvbuf(stdout, NULL, _IOLBF, 0);

    struct sigaction sa = {0};
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGCONT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    printf("Process %d: has been started\n", process_idx, pid);

    long next_n = big_n();

    printf("Process %d: will find the next triangular number from (%ld, inf)\n", process_idx, pid, next_n);

    tris = 0;

    while (1) {
        next_n++;

        if (is_triangular((int)next_n)) {
            printf("Process %d: I found this triangular number %ld\n", process_idx, pid, next_n);
            tris++;
        }
    }
}
