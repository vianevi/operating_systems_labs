#include <stdio.h>
#include <string.h>

void convert(long long x, int s, int t)
{
    if (s < 2 || s > 10 || t < 2 || t > 10 || x < 0) {
        printf("cannot convert!\n");
        return;
    }

    long long value = 0, power = 1;
    for (long long rest = x; rest > 0; rest /= 10) {
        int digit = (int)(rest % 10);
        if (digit >= s) {
            printf("cannot convert!\n");
            return;
        }
        value += digit * power;
        power *= s;
    }

    if (value == 0) {
        printf("0\n");
        return;
    }
    char result[70] = "";
    char tmp[80];
    while (value > 0) {
        sprintf(tmp, "%d%s", (int)(value % t), result);
        strcpy(result, tmp);
        value /= t;
    }
    printf("%s\n", result);
}

int main(void)
{
    long long x;
    int s, t;
    printf("Enter the number, the source base and the target base:\n");
    if (scanf("%lld %d %d", &x, &s, &t) != 3) {
        printf("cannot convert!\n");
        return 1;
    }
    convert(x, s, t);
    return 0;
}
