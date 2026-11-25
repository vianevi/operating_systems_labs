#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MSG_SIZE 1024

int main(void)
{
    int fd[2];
    if (pipe(fd) == -1) { 
        perror("pipe");
        return EXIT_FAILURE; 
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE; }

    if (pid == 0) {
        close(fd[1]);
        FILE *in = fdopen(fd[0], "r");
        char buf[MSG_SIZE + 1];

        while (fgets(buf, sizeof buf, in) != NULL) {
            printf("Subscriber received: %s", buf);
            fflush(stdout);
        }
        fclose(in);
        return EXIT_SUCCESS;
    }

    close(fd[0]);
    char msg[MSG_SIZE];
    
    printf("Publisher: type messages (Ctrl+D to finish)\n");

    while (fgets(msg, sizeof msg, stdin) != NULL)
        write(fd[1], msg, strlen(msg));
    
    close(fd[1]);
    wait(NULL);
    return EXIT_SUCCESS;
}
