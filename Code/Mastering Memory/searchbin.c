#define _GNU_SOURCE   
#include <stdio.h>
#include <string.h>

int main(void)
{
    unsigned char data[] = {
        0x10, 0x00, 0x22, 0x33, 0x00,
        0x44, 0x55, 0x00, 0x66
    };

    size_t len = sizeof(data);

    unsigned char *first = memchr(data, 0x00, len);
    unsigned char *last = memrchr(data, 0x00, len);

    if (first)
        printf("First  0x00 at index %ld\n", first - data);
    else
        printf("First  0x00 not found\n");

    if (last)
        printf("Last   0x00 at index %ld\n", last - data);
    else
        printf("Last   0x00 not found\n");

    return 0;
}
