#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MEM_SIZE 10000000u
#define MAX_QUERIES 2000000

static unsigned int mem[MEM_SIZE];
static unsigned int tag[MEM_SIZE];

typedef struct { int is_alloc; unsigned int adrs, size; } Query;
static Query queries[MAX_QUERIES];
static int nq = 0;
static int failed = 0;

static void make_block(unsigned int start, unsigned int len)
{
    tag[start] = len;
    tag[start + len - 1] = len;
}

static void reset_memory(void)
{
    memset(mem, 0, sizeof mem);
    make_block(0, MEM_SIZE);
}


static void place(unsigned int adrs, unsigned int start, unsigned int size)
{
    unsigned int len = tag[start];
    if (len > size)
        make_block(start + size, len - size);
    make_block(start, size);

    for (unsigned int i = start; i < start + size; i++)
        mem[i] = adrs;
}

void allocate_first(unsigned int adrs, unsigned int size)
{
    for (unsigned int i = 0; i < MEM_SIZE; i += tag[i])
        if (mem[i] == 0 && tag[i] >= size) {
            place(adrs, i, size);
            return;
        }
    failed++;
    (void)adrs;
}

void allocate_best(unsigned int adrs, unsigned int size)
{
    long best = -1;

    for (unsigned int i = 0; i < MEM_SIZE; i += tag[i])
        if (mem[i] == 0 && tag[i] >= size && (best == -1 || tag[i] < tag[best]))
            best = i;
            
    if (best == -1) {
        failed++;
        return;
    }
    place(adrs, (unsigned int)best, size);
}

void allocate_worst(unsigned int adrs, unsigned int size)
{
    long worst = -1;
    for (unsigned int i = 0; i < MEM_SIZE; i += tag[i])

        if (mem[i] == 0 && (worst == -1 || tag[i] > tag[worst]))
            worst = i;

    if (worst == -1 || tag[worst] < size) {
        failed++;
        return;
    }

    place(adrs, (unsigned int)worst, size);
}


void clear(unsigned int adrs)
{
    unsigned int i = 0;
    while (i < MEM_SIZE) {
        unsigned int len = tag[i];

        if (mem[i] != adrs) {
            i += len;
            continue;
        }

        for (unsigned int k = i; k < i + len; k++)
            mem[k] = 0;
        unsigned int start = i;

        if (start + len < MEM_SIZE && mem[start + len] == 0) {
            len += tag[start + len];
        }

        if (start > 0 && mem[start - 1] == 0) {
            unsigned int left = tag[start - 1];
            start -= left;
            len += left;
        }
        make_block(start, len);
        i = start + len;
    }
}

static int load_queries(const char *name)
{
    FILE *f = fopen(name, "r");

    if (f == NULL) {
        perror(name);
        return 0;
    }

    char cmd[32];
    
    while (nq < MAX_QUERIES && fscanf(f, "%31s", cmd) == 1) {
        if (strcmp(cmd, "end") == 0) {
            break;
        }

        Query q = {0};

        if (strcmp(cmd, "allocate") == 0) {
            q.is_alloc = 1;
            if (fscanf(f, "%u %u", &q.adrs, &q.size) != 2) {
                break;
            }

        } else if (strcmp(cmd, "clear") == 0) {
            if (fscanf(f, "%u", &q.adrs) != 1) {
                break;
            }
        } else {
            continue;
        }
        queries[nq++] = q;
    }
    fclose(f);
    return 1;
}

static void run(const char *name, void (*allocate)(unsigned int, unsigned int))
{
    reset_memory();
    failed = 0;
    clock_t start = clock();

    for (int i = 0; i < nq; i++) {
        if (queries[i].is_alloc) {
            allocate(queries[i].adrs, queries[i].size);
        } else {
            clear(queries[i].adrs);
        }
    }
    double sec = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("%-10s: %d queries in %.4f s -> throughput = %.0f queries/s (%d allocations did not fit)\n", name, nq, sec, sec > 0 ? nq / sec : 0.0, failed);
}

int main(void)
{
    if (!load_queries("queries.txt")){
        return EXIT_FAILURE;
    }

    run("First fit", allocate_first);
    run("Best fit",  allocate_best);
    run("Worst fit", allocate_worst);

    return EXIT_SUCCESS;
}
