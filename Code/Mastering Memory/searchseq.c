#define _GNU_SOURCE  
#include <stdio.h>
#include <string.h>

int main(void)
{
    const unsigned char haystack[] = {
        0x10, 0x20, 0x30, 0x40,
        0x50, 0x60, 0x70, 0x80
    };

    const unsigned char needle[] = {
        0x40, 0x50, 0x60
    };

    size_t haystack_len = sizeof(haystack);
    size_t needle_len   = sizeof(needle);

    unsigned char *p = memmem(haystack, haystack_len, needle, needle_len);

    if (p != NULL) {
        printf("Pattern found at index %ld\n", p - haystack);
    } else {
        printf("Pattern not found\n");
    }

    return 0;
}
