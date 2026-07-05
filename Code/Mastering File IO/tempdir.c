#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char template[] = "/tmp/mydirXXXXXX";

    char *dirname = mkdtemp(template);
    if (dirname == NULL) {
        perror("mkdtemp");
        return 1;
    }

    printf("Temporary directory created: %s\n", dirname);

  
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/tempfile.txt", dirname);

    FILE *fp = fopen(filepath, "w");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }
    fprintf(fp, "Hello from mkdtemp!\n");
    fclose(fp);

    printf("Created a file: %s\n", filepath);

    return 0;
}
