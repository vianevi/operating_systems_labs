#include <stdio.h>
#include <string.h>

#define MAX_LEN 256

int main(void)
{
    char s[MAX_LEN + 1];
    int  n = 0;
    int  ch;

    printf("Enter a string (finish with '.' or Enter):\n");
    while (n < MAX_LEN) {
        ch = getchar();
        if (ch == EOF || ch == '.' || ch == '\n')
            break;
        s[n++] = (char)ch;
    }
    s[n] = '\0';

    putchar('"');
    for (int i = (int)strlen(s) - 1; i >= 0; i--)
        putchar(s[i]);
    printf("\"\n");
    return 0;
}
