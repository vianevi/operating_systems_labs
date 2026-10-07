#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;
    clock_t start, end;
    double cpu_time_ms;

    pid1 = fork();

    if (pid1 < 0) {
        perror("fork 1 failed");
        return 1;
    }

    if (pid1 == 0) {
        start = clock();



        end = clock();
        cpu_time_ms = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;

        printf("Child 1: PID=%d, PPID=%d, time=%.3f ms\n",
                getpid(), getppid(), cpu_time_ms);

        return 0;
    }


    pid2 = fork();

    if (pid2 < 0) {
        perror("fork 2 failed");
        return 1;
    }

    if (pid2 == 0) {
        start = clock();



        end = clock();
        cpu_time_ms = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;

        printf("Child 2: PID=%d, PPID=%d, time=%.3f ms\n",
                getpid(), getppid(), cpu_time_ms);

        return 0;
    }

    start = clock();

    wait(NULL);
    wait(NULL);

    end = clock();
    cpu_time_ms = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    printf("Parent: PID=%d, PPID=%d, time=%.3f ms\n",
            getpid(), getppid(), cpu_time_ms);

    return 0;
}