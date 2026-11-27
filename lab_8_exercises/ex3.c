#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#define MB (1024 * 1024)

int main(void)
{
    void *blocks[10];
    struct rusage usage;
    
    for (int i = 0; i < 10; i++) {
        blocks[i] = malloc(10 * MB);
        if (blocks[i] == NULL) {
            perror("malloc");
            return 1;
        }

        memset(blocks[i], 0, 10 * MB);
        getrusage(RUSAGE_SELF, &usage);
        printf("second %2d: max resident set size = %ld KB, minor page faults = %ld, major = %ld\n", i + 1, usage.ru_maxrss, usage.ru_minflt, usage.ru_majflt);
        fflush(stdout);
        sleep(1);
    }
    for (int i = 0; i < 10; i++) free(blocks[i]);
    return 0;
}
