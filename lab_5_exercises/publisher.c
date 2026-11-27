#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MSG_SIZE 1024
#define MAX_SUBS 32

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s n\n", argv[0]); 
        return 1;
    }

    int n = atoi(argv[1]);

    if (n < 1 || n > MAX_SUBS) {
        fprintf(stderr, "1 <= n <= %d\n", MAX_SUBS);
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);
    mkdir("/tmp/ex1", 0777);

    int to_child[MAX_SUBS][2];
    
    for (int i = 1; i <= n; i++) {
        char fifo[64];
        snprintf(fifo, sizeof fifo, "/tmp/ex1/s%d", i);

        if (mkfifo(fifo, 0666) == -1 && errno != EEXIST) {
            perror("mkfifo");
            return 1;
        }

        if (pipe(to_child[i - 1]) == -1) {
            perror("pipe"); 
            return 1;
        }
        pid_t pid = fork();

        if (pid == 0) {
            close(to_child[i - 1][1]);
            for (int k = 0; k < i - 1; k++) {
                close(to_child[k][1]);
            }

            int out = open(fifo, O_WRONLY);
            if (out == -1) {
                perror("open fifo");
                _exit(1);
            }

            char buf[MSG_SIZE];
            ssize_t r;

            while ((r = read(to_child[i - 1][0], buf, sizeof buf)) > 0)
                write(out, buf, (size_t)r);
            close(out);
            _exit(0);
        }
        close(to_child[i - 1][0]);
    }

    char msg[MSG_SIZE];

    printf("Publisher: %d subscribers. Type messages\n", n);
    fflush(stdout);

    while (fgets(msg, sizeof msg, stdin) != NULL)
        for (int i = 0; i < n; i++)
            write(to_child[i][1], msg, strlen(msg));

    for (int i = 0; i < n; i++) {
        close(to_child[i][1]);
    }
    
    while (wait(NULL) > 0);
    return 0;
}
