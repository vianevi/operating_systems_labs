#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PT_FILE "/tmp/ex2/pagetable"

struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
};

static struct PTE *pt;
static int P;
static volatile sig_atomic_t resumed = 0;

static void on_sigcont(int signum) {
    (void)signum;
    resumed = 1;
}

static void print_page_table(void)
{
    printf("Page table\n");
    for (int p = 0; p < P; p++)
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n", p, pt[p].valid, pt[p].frame, pt[p].dirty, pt[p].referenced);
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "Usage: %s pages refs... pager_pid\n", argv[0]);
        return 1;
    }

    P = atoi(argv[1]);
    pid_t pager = (pid_t)atoi(argv[argc - 1]);
    setvbuf(stdout, NULL, _IOLBF, 0);

    int fd = open(PT_FILE, O_RDWR);
    if (fd == -1) {
        perror(PT_FILE " (start the pager first)");
        return 1;
    }

    size_t pt_size = (size_t)P * sizeof(struct PTE);
    pt = mmap(NULL, pt_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (pt == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    struct sigaction sa = {0};
    sa.sa_handler = on_sigcont;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCONT, &sa, NULL);
    sigset_t block, old;
    sigemptyset(&block);
    sigaddset(&block, SIGCONT);
    sigprocmask(SIG_BLOCK, &block, &old);

    printf("-------------------------\nInitialized page table\n");
    print_page_table();

    for (int i = 2; i < argc - 1; i++) {
        char mode = argv[i][0];
        int page = atoi(argv[i] + 1);
        if ((mode != 'R' && mode != 'W') || page < 0 || page >= P) {
            printf("Skipping invalid request \"%s\"\n", argv[i]);
            continue;
        }
        printf("-------------------------\n%s Request for page %d\n", mode == 'W' ? "Write" : "Read", page);

        if (!pt[page].valid) {
            printf("It is not a valid page --> page fault\n");
            printf("Ask pager to load it from disk (SIGUSR1 signal) and wait\n");
            pt[page].referenced = getpid();
            resumed = 0;
            kill(pager, SIGUSR1);
            while (!resumed)
                sigsuspend(&old);
            printf("MMU resumed by SIGCONT signal from pager\n");
        } else {
            printf("It is a valid page\n");
        }

        if (mode == 'W') {
            printf("It is a write request then set the dirty field\n");
            pt[page].dirty = true;
        }
        print_page_table();
    }

    printf("Done all requests.\n");
    printf("MMU sends SIGUSR1 to the pager.\n");
    munmap(pt, pt_size);
    close(fd);
    kill(pager, SIGUSR1);
    printf("MMU terminates.\n");
    return 0;
}
