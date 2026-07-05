#include <stdio.h>
#include <string.h>

int main(void)
{
    unsigned char a[] = { 0x01, 0x02, 0x03, 0x04 };
    unsigned char b[] = { 0x01, 0x02, 0x05, 0x04 };

    size_t n = sizeof(a);

    int ret = memcmp(a, b, n);

    if (ret == 0) {
        printf("The memory blocks are equal\n");
    } else if (ret < 0) {
        printf("The first memory block is less than the second\n");
    } else {
        printf("The first memory block is greater than the second\n");
    }

    return 0;
}
