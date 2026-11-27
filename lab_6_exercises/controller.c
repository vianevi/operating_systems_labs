#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#define PID_FILE "/var/run/agent.pid"

static pid_t agent_pid = -1;

static void on_sigint(int signum)
{
    (void)signum;
    kill(agent_pid, SIGTERM);
    const char msg[] = "\nController: sent SIGTERM to the agent, bye\n";

    write(STDOUT_FILENO, msg, sizeof msg - 1);
    _exit(EXIT_SUCCESS);
}

int main(void)
{
    FILE *pf = fopen(PID_FILE, "r");
    if (pf == NULL || fscanf(pf, "%d", &agent_pid) != 1 || (kill(agent_pid, 0) == -1 && errno != EPERM)) {
        printf("Error: No agent found.\n");

        if (pf) {
            fclose(pf);
        }

        return EXIT_FAILURE;
    }

    fclose(pf);
    printf("Agent found.\n");

    signal(SIGINT, on_sigint);

    char cmd[64];
    while (1) {
        printf("Choose a command {\"read\", \"exit\", \"stop\", \"continue\"}\n");
        fflush(stdout);

        if (scanf("%63s", cmd) != 1) {
            break;
        }

        if (strcmp(cmd, "read") == 0) {
            kill(agent_pid, SIGUSR1);
        } else if (strcmp(cmd, "stop") == 0) {
            kill(agent_pid, SIGSTOP);
        } else if (strcmp(cmd, "continue") == 0) {
            kill(agent_pid, SIGCONT);
        } else if (strcmp(cmd, "exit") == 0) {
            kill(agent_pid, SIGUSR2);
            break;
        } else
            printf("Unknown command \"%s\"\n", cmd);
    }
    return EXIT_SUCCESS;
}
