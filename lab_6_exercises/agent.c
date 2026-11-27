#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define PID_FILE "/var/run/agent.pid"
#define TEXT_FILE "text.txt"

static void print_text(void)
{
    FILE *f = fopen(TEXT_FILE, "r");
    if (f == NULL) {
        perror(TEXT_FILE);
        return;
    
    }
    char line[1024];

    while (fgets(line, sizeof line, f) != NULL)
        fputs(line, stdout);
    fclose(f);
    fflush(stdout);
}

static void handler(int signum)
{
    if (signum == SIGUSR1) {
        print_text();
    } else {
        if (signum == SIGUSR2) {
            printf("Process terminating...\n");
        }
        fflush(stdout);
        unlink(PID_FILE);
        exit(EXIT_SUCCESS);
    }
}

int main(void)
{
    FILE *pf = fopen(PID_FILE, "w");
    if (pf == NULL) {
        return EXIT_FAILURE;
    }
    fprintf(pf, "%d\n", getpid());
    fclose(pf);

    struct sigaction sa = {0};
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    print_text();

    while (1)
        pause();
}
