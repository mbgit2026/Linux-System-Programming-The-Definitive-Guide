#include <stdio.h>
#include <err.h>

int main(void) {
    FILE *fp = fopen("nonexistent.txt", "r");
    if (fp == NULL) {
        err(1, "failed to open file");
    }

    printf("This will never be printed.\n");
    fclose(fp);
    return 0;
}
