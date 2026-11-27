#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>

#define PASS_LEN 8

int main(void)
{
    FILE *pf = fopen("/tmp/ex1.pid", "w");
    if (pf == NULL) {
        perror("/tmp/ex1.pid");
        
        return EXIT_FAILURE;
    }

    fprintf(pf, "%d\n", getpid());
    fclose(pf);

    char *password = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (password == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    FILE *rnd = fopen("/dev/urandom", "r");

    if (rnd == NULL) {
        perror("/dev/urandom");

        return EXIT_FAILURE;
    }
    memcpy(password, "pass:", 5);
    int n = 5;
    while (n < 5 + PASS_LEN) {
        int c = fgetc(rnd);

        if (isprint(c) && c != ' ')
            password[n++] = (char)c;
    }
    password[n] = '\0';
    fclose(rnd);

    printf("ex1 (PID %d) is running; the password is hidden in memory\n", getpid());
    fflush(stdout);
    while (1)
        sleep(1);
}
