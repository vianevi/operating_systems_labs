#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 256

int count(const char str[], char ch)
{
    int c = 0;
    ch = (char)tolower((unsigned char)ch);
    for (int i = 0; str[i] != '\0'; i++)
        if (tolower((unsigned char)str[i]) == ch)
            c++;
    return c;
}

void countAll(const char str[])
{
    int len = (int)strlen(str);
    for (int i = 0; i < len; i++) {
        char c = (char)tolower((unsigned char)str[i]);
        printf("%c:%d", c, count(str, c));
        printf(i < len - 1 ? ", " : "\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }
    char str[MAX_LEN + 1];
    strncpy(str, argv[1], MAX_LEN);
    str[MAX_LEN] = '\0';
    countAll(str);
    return 0;
}
