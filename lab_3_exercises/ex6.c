#include <stdio.h>

static void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

static int partition(int arr[], int lo, int hi)
{
    int pivot = arr[hi];
    int i = lo;

    for (int j = lo; j < hi; j++)
        if (arr[j] < pivot)
            swap(&arr[i++], &arr[j]);
    swap(&arr[i], &arr[hi]);
    return i;
}

void quicksort(int arr[], int lo, int hi)
{
    if (lo >= hi) {
        return;
    }

    int p = partition(arr, lo, hi);
    quicksort(arr, lo, p - 1);
    quicksort(arr, p + 1, hi);
}

static void print(const char *title, int a[], int n)
{
    printf("%s", title);

    for (int i = 0; i < n; i++) {
        printf(" %d", a[i]);
    }
    
    printf("\n");
}

int main(void)
{
    int a[] = {29, 3, 72, 15, -4, 88, 15, 0, 41, 7};
    int n = (int)(sizeof(a) / sizeof(a[0]));

    print("before:", a, n);
    quicksort(a, 0, n - 1);
    print("after: ", a, n);
    return 0;
}
