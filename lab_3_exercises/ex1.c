#include <stdio.h>
#include <stdlib.h>

int const_tri(int *const p, int n)
{
    int temp;
    if (n == 0) {
        return 0;
    }

    if (n <= 2) {
        return 1;
    }

    p[0] = 0; p[1] = 1; p[2] = 1;

    while (n > 2) {
        temp = p[0] + p[1] + p[2];
        p[0] = p[1];
        p[1] = p[2];
        p[2] = temp;
        n--;
    }
    return p[2];
}

int main(void)
{
    const int x = 1;
    const int *q = &x;

    int *const p = malloc(3 * sizeof(int));
    if (p == NULL) {
        perror("malloc");
        return 1;
    }

    p[0] = *q;
    p[1] = *q;
    p[2] = 2 * (*q);

    for (int i = 0; i < 3; i++)
        printf("cell %d: address = %p, value = %d\n", i, (void *)&p[i], p[i]);

    if ((char *)&p[1] - (char *)&p[0] == sizeof(int) && (char *)&p[2] - (char *)&p[1] == sizeof(int))
        printf("The cells are contiguous (step = %zu bytes)\n", sizeof(int));
    else
        printf("The cells are NOT contiguous\n");

    printf("const_tri(4)  = %d\n", const_tri(p, 4));
    printf("const_tri(36) = %d\n", const_tri(p, 36));

    free(p);
    return 0;
}
