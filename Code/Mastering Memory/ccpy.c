#include <stdio.h>
#include <string.h>

int main(void)
{
    char src[] = "Hello:world:example";
    char dst[32];

    void *p = memccpy(dst, src, ':', sizeof(dst));

    if (p != NULL) {
            size_t copied = (char *)p - dst;
        printf("Copied %zu bytes (including ':')\n", copied);
    } else {
        printf("Character ':' not found\n");
    }

    /* Print what was copied */
    fwrite(dst, 1, (p ? (size_t)((char *)p - dst) : sizeof(dst)), stdout);
    putchar('\n');

    return 0;
}
