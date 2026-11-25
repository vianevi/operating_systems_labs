#include <stdio.h>

int tribonacci(int n)
{
    if (n == 0) {
        return 0;
    }

    if (n <= 2) {
        return 1;
    }

    int a = 0, b = 1, c = 1;
    
    for (int i = 3; i <= n; i++) {
        int next = a + b + c;
        a = b;
        b = c;
        c = next;
    }
    return c;
}

int main(void)
{
    printf("T(4)  = %d\n", tribonacci(4));
    printf("T(36) = %d\n", tribonacci(36));
    return 0;
}
