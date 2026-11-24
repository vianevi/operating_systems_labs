#include <stdio.h>
#include <limits.h>
#include <float.h>

int main(void)
{
    int a = INT_MAX;
    unsigned short int b = USHRT_MAX;
    signed long int c = LONG_MAX;
    float d = FLT_MAX;
    double e = DBL_MAX;

    printf("int: size = %zu bytes, max = %d\n",  sizeof(a), a);
    printf("unsigned short int: size = %zu bytes, max = %hu\n", sizeof(b), b);
    printf("signed long int: size = %zu bytes, max = %ld\n", sizeof(c), c);
    printf("float: size = %zu bytes, max = %e\n",  sizeof(d), d);
    printf("double: size = %zu bytes, max = %e\n",  sizeof(e), e);
    return 0;
}
