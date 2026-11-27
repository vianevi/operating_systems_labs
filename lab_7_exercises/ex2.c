#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define FILE_NAME "text.txt"
#define LINE_LEN 1024

int main(int argc, char *argv[])
{
    long mib = (argc > 1) ? atol(argv[1]) : 500;
    off_t file_size = (off_t)mib * 1024 * 1024;
    long page = sysconf(_SC_PAGESIZE);
    size_t chunk = (size_t)page * 1024;


    FILE *rnd = fopen("/dev/random", "r");
    int fd = open(FILE_NAME, O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (rnd == NULL || fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (ftruncate(fd, file_size) == -1) {
        perror("ftruncate");
        return EXIT_FAILURE;
    }

    long col = 0;

    for (off_t off = 0; off < file_size; off += (off_t)chunk) {
        size_t len = (size_t)(file_size - off) < chunk ? (size_t)(file_size - off) : chunk;
        char *p = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, off);

        if (p == MAP_FAILED) {
            perror("mmap");
            return EXIT_FAILURE;
        }

        for (size_t i = 0; i < len; i++) {
            if (col == LINE_LEN) {
                p[i] = '\n'; col = 0;
                continue;
            }

            int c;
            
            do c = fgetc(rnd); while (!isprint(c));
            p[i] = (char)c;
            col++;
        }
        munmap(p, len);
    }
    fclose(rnd);
    printf("Created %s: %lld bytes\n", FILE_NAME, (long long)file_size);


    long long capitals = 0;
    for (off_t off = 0; off < file_size; off += (off_t)chunk) {
        
        size_t len = (size_t)(file_size - off) < chunk ? (size_t)(file_size - off) : chunk;
        char *p = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, off);

        if (p == MAP_FAILED) {
            perror("mmap");
            return EXIT_FAILURE;
        }
        for (size_t i = 0; i < len; i++)
            if (isupper((unsigned char)p[i])) {
                capitals++;
                p[i] = (char)tolower((unsigned char)p[i]);
            }
        munmap(p, len);
    }

    close(fd);
    printf("Total number of capital letters: %lld\n", capitals);
    return EXIT_SUCCESS;
}
