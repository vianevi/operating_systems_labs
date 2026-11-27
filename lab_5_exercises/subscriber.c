#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MSG_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s id\n", argv[0]);
        return 1;
    }

    char fifo[64];
    snprintf(fifo, sizeof fifo, "/tmp/ex1/s%s", argv[1]);

    int in = open(fifo, O_RDONLY);

    if (in == -1) {
        perror("open fifo");
        return 1;
    }

    FILE *f = fdopen(in, "r");
    char buf[MSG_SIZE + 1];
    while (fgets(buf, sizeof buf, f) != NULL) {
        
        printf("Subscriber %s: %s", argv[1], buf);
        fflush(stdout);
    }
    fclose(f);
    return 0;
}
