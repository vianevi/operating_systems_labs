#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>

void *vaggregate(size_t size, void *initial_value, void *(*opr)(const void *, const void *), int n, ...)
{
    void *acc = malloc(size);
    if (acc == NULL) {
        return NULL;
    }

    memcpy(acc, initial_value, size);

    va_list ap;
    va_start(ap, n);
    
    for (int i = 0; i < n; i++) {
        void *r;
        if (size == sizeof(int)) {
            int v = va_arg(ap, int);
            r = opr(acc, &v);
        } else {
            double v = va_arg(ap, double);
            r = opr(acc, &v);
        }
        memcpy(acc, r, size);
    }
    va_end(ap);
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
    int zero_i = 0, one_i = 1, min_i = INT_MIN;
    double zero_d = 0.0, one_d = 1.0, min_d = -DBL_MAX;
    int *r;
    double *d;

    r = vaggregate(sizeof(int), &zero_i, add_int, 5, 1, 2, 3, 4, 5); printf("int sum = %d\n", *r); free(r);
    r = vaggregate(sizeof(int), &one_i, mul_int, 5, 1, 2, 3, 4, 5); printf("int product = %d\n", *r); free(r);
    r = vaggregate(sizeof(int), &min_i, max_int, 5, 1, 2, 3, 4, 5); printf("int max = %d\n", *r); free(r);

    
    d = vaggregate(sizeof(double), &zero_d, add_double, 5, 1.5, 2.5, -3.0, 4.0, 0.5); printf("double sum = %f\n", *d); free(d);
    d = vaggregate(sizeof(double), &one_d, mul_double, 5, 1.5, 2.5, -3.0, 4.0, 0.5); printf("double product = %f\n", *d); free(d);
    d = vaggregate(sizeof(double), &min_d, max_double, 5, 1.5, 2.5, -3.0, 4.0, 0.5); printf("double max = %f\n", *d); free(d);
    return 0;
}
