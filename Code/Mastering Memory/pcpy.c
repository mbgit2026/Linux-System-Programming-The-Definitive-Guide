#define _GNU_SOURCE   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *combine(void *o1, size_t s1, void *o2, size_t s2)
{
    void *result = malloc(s1 + s2);
    if (result != NULL)
        mempcpy(mempcpy(result, o1, s1), o2, s2);
    return result;
}

int main(void)
{
    const char part1[] = "Hello, ";
    const char part2[] = "world!\n";

    size_t s1 = sizeof(part1) - 1; 
    size_t s2 = sizeof(part2) - 1;

    char *combined = combine((void *)part1, s1,
                             (void *)part2, s2);
    if (combined == NULL) {
        perror("malloc");
        return 1;
    }

    fwrite(combined, 1, s1 + s2, stdout);
    free(combined);
    return 0;
}
