#include <stdio.h>
#include <string.h>

int main(void)
{
    char buffer[16];

    memset(buffer, 0, sizeof(buffer));
    memset(buffer, 'a', 8);

    for (size_t i = 0; i < sizeof(buffer); i++) {
        printf("%02x ", (unsigned char)buffer[i]);
    }
    putchar('\n');

    printf("As string: \"%s\"\n", buffer);
    return 0;
}
