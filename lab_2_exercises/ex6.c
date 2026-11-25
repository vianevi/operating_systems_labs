#include <stdio.h>
#include <stdlib.h>

static void stars(int k) { for (int i = 0; i < k; i++) putchar('*'); putchar('\n'); }

int main(int argc, char *argv[])
{
    int n = (argc > 1) ? atoi(argv[1]) : 7;

    for (int r = 1; r <= n; r++) stars(r);
    putchar('\n');

    for (int r = 1; r <= n; r++) {
        int k = (r <= (n + 1) / 2) ? r : n + 1 - r;
        stars(k);
    }
    putchar('\n');

    for (int r = 1; r <= n; r++) stars(n);
    return 0;
}
