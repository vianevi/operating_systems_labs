#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>

#define PS_MAX 10
#ifndef TICK_USEC
#define TICK_USEC 1000000
#endif

typedef struct {
    int idx;
    int at, bt, rt, wt, ct, tat;
    int burst;
} ProcessData;


int running_process = -1;
unsigned total_time;
ProcessData data[PS_MAX];
pid_t ps[PS_MAX];
unsigned data_size;


void read_file(FILE *file)
{
    char header[128];
    if (fgets(header, sizeof header, file) == NULL) {
        return;
    }
    
    int idx, at, bt;

    while (data_size < PS_MAX && fscanf(file, "%d %d %d", &idx, &at, &bt) == 3) {
        if (idx < 0 || idx >= PS_MAX) {
            continue;
        }
        
        data[idx] = (ProcessData){ .idx = idx, .at = at, .bt = bt, .burst = bt };
        ps[idx] = 0;
        data_size++;
    }
}


void resume(pid_t process) {
    if (process > 0) {
        kill(process, SIGCONT);
    }

}
void suspend(pid_t process) {
    if (process > 0) {
        kill(process, SIGTSTP);
    }
}

void terminate(pid_t process) {
    if (process > 0) {
        kill(process, SIGTERM);
    }
}


void create_process(int new_process)
{
    if (running_process != -1)
        suspend(ps[running_process]);

    pid_t pid = fork();
    if (pid == 0) {
        sigset_t none;
        sigemptyset(&none);
        sigprocmask(SIG_SETMASK, &none, NULL);
        char arg[16];
        snprintf(arg, sizeof arg, "%d", new_process);
        char *args[] = { "./worker", arg, NULL };
        execvp(args[0], args);
        perror("execvp ./worker");
        _exit(EXIT_FAILURE);
    }
    ps[new_process] = pid;
}


ProcessData find_next_process()
{
    while (1) {
        int location = -1;
        for (unsigned i = 0; i < data_size; i++)
            if (data[i].burst > 0 && (location == -1 || data[i].at < data[location].at))
                location = (int)i;

        if (location == -1)
            return (ProcessData){ .idx = -1 };

        if ((unsigned)data[location].at > total_time) {
            printf("Process %d: has not arrived yet.\n", location);
            total_time++;
            continue;
        }
        return data[location];
    }
}


void report()
{
    printf("Simulation results.....\n");

    int sum_wt = 0, sum_tat = 0;
    
    for (unsigned i = 0; i < data_size; i++) {
        printf("process %u: \n", i);
        printf("\tat=%d\n", data[i].at);
        printf("\tbt=%d\n", data[i].bt);
        printf("\tct=%d\n", data[i].ct);
        printf("\twt=%d\n", data[i].wt);
        printf("\ttat=%d\n", data[i].tat);
        printf("\trt=%d\n", data[i].rt);
        sum_wt += data[i].wt;
        sum_tat += data[i].tat;
    }
    printf("data size = %u\n", data_size);
    printf("Average results for this run:\n");
    printf("\tavg_wt=%f\n", (float)sum_wt / data_size);
    printf("\tavg_tat=%f\n", (float)sum_tat / data_size);
}


void check_burst()
{
    for (unsigned i = 0; i < data_size; i++)
        if (data[i].burst > 0)
            return;
    report();
    exit(EXIT_SUCCESS);
}


void schedule_handler(int signum)
{
    (void)signum;
    total_time++;
    printf("Scheduler: Runtime: %u seconds.\n", total_time);

    if (running_process != -1) {
        ProcessData *p = &data[running_process];
        p->burst--;
        printf("Scheduler: Process %d is running with %d seconds left\n", p->idx, p->burst);
        if (p->burst == 0) {
            printf("Scheduler: Terminating Process %d. Remaining Time: %d\n", p->idx, p->burst);
            terminate(ps[running_process]);
            waitpid(ps[running_process], NULL, 0);
            p->ct = (int)total_time;
            p->tat = p->ct - p->at;
            p->wt = p->tat - p->bt;
            running_process = -1;
        }
    }

    check_burst();

    if (running_process == -1) {
        ProcessData next = find_next_process();
        if (next.idx != -1) {
            printf("Scheduler: Starting Process %d (Remaining Time: %d)\n", next.idx, next.burst);
            data[next.idx].rt = (int)total_time - data[next.idx].at;
            create_process(next.idx);
            running_process = next.idx;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s data.txt\n", argv[0]);
        return 1;
    }

    setvbuf(stdout, NULL, _IOLBF, 0);
    FILE *in = fopen(argv[1], "r");
    if (in == NULL) {
        perror(argv[1]);
        return 1;
    }

    read_file(in);
    fclose(in);

    struct sigaction sa = {0};
    sa.sa_handler = schedule_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);

    struct itimerval timer;
    timer.it_value.tv_sec = TICK_USEC / 1000000;
    timer.it_value.tv_usec = TICK_USEC % 1000000;
    timer.it_interval = timer.it_value;
    setitimer(ITIMER_REAL, &timer, NULL);

    while (1){
        pause();
    }
}
