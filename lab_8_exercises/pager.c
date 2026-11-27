#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define PT_DIR "/tmp/ex2"
#define PT_FILE "/tmp/ex2/pagetable"
#define PAGE_STR 8

struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
};

static struct PTE *pt;
static int P, F;
static char (*ram)[PAGE_STR];
static char (*disk)[PAGE_STR];
static int *frame_page;
static int disk_accesses = 0;
static volatile sig_atomic_t request = 0;

static void on_sigusr1(int signum) { (void)signum; request = 1; }

static void print_page_table(void)
{
    printf("Page table\n");
    for (int p = 0; p < P; p++)
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n", p, pt[p].valid, pt[p].frame, pt[p].dirty, pt[p].referenced);
}

static void print_ram(void)
{
    printf("RAM array\n");
    for (int f = 0; f < F; f++)
        printf("Frame %d ---> %s\n", f, ram[f]);
}


static bool handle_request(void)
{
    int page = -1;
    for (int p = 0; p < P; p++)
        if (pt[p].referenced != 0) {
            page = p;
            break;
        }

    if (page == -1)
        return false;

    pid_t mmu = pt[page].referenced;
    printf("-------------------------\n");
    printf("A disk access request from MMU Process (pid=%d)\n", mmu);
    printf("Page %d is referenced\n", page);

    int frame = -1;
    for (int f = 0; f < F; f++)
        if (frame_page[f] == -1) {
            frame = f;
            break;
        }

    if (frame != -1) {
        printf("We can allocate it to free frame %d\n", frame);
    } else {
        printf("We do not have free frames in RAM\n");
        frame = rand() % F;
        int victim = frame_page[frame];

        printf("Chose a random victim page %d\n", victim);
        printf("Replace/Evict it with page %d to be allocated to frame %d\n", page, frame);

        if (pt[victim].dirty) {
            printf("Page %d is dirty: write it back to the disk\n", victim);
            memcpy(disk[victim], ram[frame], PAGE_STR);
            disk_accesses++;
        }
        pt[victim].valid = false;
        pt[victim].frame = -1;
        pt[victim].dirty = false;
    }

    printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", page, frame);
    memcpy(ram[frame], disk[page], PAGE_STR);
    disk_accesses++;
    frame_page[frame] = page;

    pt[page].valid = true;
    pt[page].frame = frame;
    pt[page].dirty = false;
    pt[page].referenced = 0;

    print_ram();
    printf("disk accesses is %d so far\n", disk_accesses);
    printf("Resume MMU process\n");
    fflush(stdout);
    kill(mmu, SIGCONT);
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s pages frames\n", argv[0]);
        return 1;
    }

    P = atoi(argv[1]);
    F = atoi(argv[2]);

    if (P <= 0 || F <= 0) {
        fprintf(stderr, "pages and frames must be positive\n");
        return 1;
    }

    if (F > P) {
        F = P;
    }
    setvbuf(stdout, NULL, _IOLBF, 0);
    srand((unsigned)time(NULL));

    struct sigaction sa = {0};
    sa.sa_handler = on_sigusr1;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
    sigset_t block, old;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);
    sigprocmask(SIG_BLOCK, &block, &old);


    mkdir(PT_DIR, 0777);
    int fd = open(PT_FILE, O_RDWR | O_CREAT | O_TRUNC, 0666);

    if (fd == -1) {
        perror(PT_FILE);
        return 1;
    }
    size_t pt_size = (size_t)P * sizeof(struct PTE);

    if (ftruncate(fd, (off_t)pt_size) == -1) {
        perror("ftruncate");
        return 1;
    }
    pt = mmap(NULL, pt_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (pt == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    for (int p = 0; p < P; p++)
        pt[p] = (struct PTE){ .valid = false, .frame = -1, .dirty = false, .referenced = 0 };


    ram = calloc((size_t)F, PAGE_STR);
    disk = calloc((size_t)P, PAGE_STR);
    frame_page = malloc((size_t)F * sizeof(int));

    for (int f = 0; f < F; f++) frame_page[f] = -1;
    const char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for (int p = 0; p < P; p++) {
        for (int i = 0; i < PAGE_STR - 1; i++)
            disk[p][i] = alphabet[rand() % (int)(sizeof alphabet - 1)];
        disk[p][PAGE_STR - 1] = '\0';
    }

    printf("Pager PID = %d\n", getpid());
    printf("-------------------------\nInitialized page table\n");
    print_page_table();
    printf("-------------------------\nInitialized RAM\n");
    print_ram();
    printf("-------------------------\nInitialized disk\nDisk array\n");
    for (int p = 0; p < P; p++) printf("Page %d ----> %s\n", p, disk[p]);

    while (1) {
        while (!request)
            sigsuspend(&old);
        request = 0;
        if (!handle_request())
            break;
    }

    printf("-------------------------\n");
    printf("%d disk accesses in total\n", disk_accesses);
    printf("Pager is terminated\n");
    munmap(pt, pt_size);
    close(fd);
    unlink(PT_FILE);
    free(ram);
    free(disk);
    free(frame_page);
    return 0;
}
