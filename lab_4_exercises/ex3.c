#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char input[1024];
    char *args[64];
    int is_background = 0;

    while (1) {
        printf("myshell> ");
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0) {
            continue;
        }

        if (strcmp(input, "exit") == 0) {
            break;
        }

        int i = 0;
        char *token = strtok(input, " ");

        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }

        args[i] = NULL;


        if (i == 0) continue;

        if (strcmp(args[0], "bg") == 0) {
            is_background = 1;

            for (int j = 0; args[j] != NULL; j++) {
                args[j] = args[j + 1];
            }
        } else {
            is_background = 0;
        }

        if (args[0] == NULL) {
            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            execvp(args[0], args);
            perror("exec failed");
            exit(1);
        } else {
            if (!is_background) {
                wait(NULL);
            } else {
                printf("[%d] background\n", pid);
            }
        }
    }

    return 0;
}