#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

void *aggregate(void *base, size_t size, int n, void *initial_value, void *(*opr)(const void *, const void *))
{
    void *acc = malloc(size);

    if (acc == NULL) {
        return NULL;
    }

    memcpy(acc, initial_value, size);

    for (int i = 0; i < n; i++) {
        const void *elem = (const char *)base + (size_t)i * size;
        void *r = opr(acc, elem);
        memcpy(acc, r, size);
    }
    return acc;
}

void *add_int(const void *a, const void *b) {
    static int r;
    r = *(const int *)a + *(const int *)b;
    return &r; }

void *mul_int(const void *a, const void *b) {
    static int r;
    r = *(const int *)a * *(const int *)b;
    return &r; }
    
void *max_int(const void *a, const void *b) {
    static int r;
    int x = *(const int *)a, y = *(const int *)b;
    r = x > y ? x : y;
    return &r; }

void *add_double(const void *a, const void *b) {
    static double r;
    r = *(const double *)a + *(const double *)b;
    return &r; }

void *mul_double(const void *a, const void *b) {
    static double r;
    r = *(const double *)a * *(const double *)b;
    return &r; }

void *max_double(const void *a, const void *b) {
    static double r;
    double x = *(const double *)a, y = *(const double *)b;
    r = x > y ? x : y;
    return &r; }

int main(void)
{
    int ai[5] = {1, 2, 3, 4, 5};
    double ad[5] = {1.5, 2.5, -3.0, 4.0, 0.5};

    int zero_i = 0, one_i = 1, min_i = INT_MIN;
    double zero_d = 0.0, one_d = 1.0, min_d = -DBL_MAX;

    int *r;
    r = aggregate(ai, sizeof(int), 5, &zero_i, add_int); printf("int sum = %d\n", *r); free(r);
    r = aggregate(ai, sizeof(int), 5, &one_i, mul_int); printf("int product = %d\n", *r); free(r);
    r = aggregate(ai, sizeof(int), 5, &min_i, max_int); printf("int max = %d\n", *r); free(r);

    double *d;
    d = aggregate(ad, sizeof(double), 5, &zero_d, add_double); printf("double sum = %f\n", *d); free(d);
    d = aggregate(ad, sizeof(double), 5, &one_d, mul_double); printf("double product = %f\n", *d); free(d);
    d = aggregate(ad, sizeof(double), 5, &min_d, max_double); printf("double max = %f\n", *d); free(d);
    return 0;
}
