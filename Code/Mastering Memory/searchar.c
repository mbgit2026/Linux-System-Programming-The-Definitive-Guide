#define _GNU_SOURCE  
#include <stdio.h>
#include <string.h>

int main(void)
{
    const char buffer[] = "abc:def:ghi:def";
    size_t len = sizeof(buffer) - 1;  /* exclude '\0' */

    char *first = memchr(buffer, ':', len);
    char *last = memrchr(buffer, ':', len);

    if (first)
        printf("First ':' at index %ld\n", first - buffer);
    else
        printf("First ':' not found\n");

    if (last)
        printf("Last ':' at index %ld\n", last - buffer);
    else
        printf("Last ':' not found\n");

    return 0;
}
