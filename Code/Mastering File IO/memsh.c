#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

extern char **environ;

int main(void)
{
    FILE *stbuff, *fpout;
    char *buffer;
    size_t size;
    char line[256];
    char s[8];

    stbuff = open_memstream(&buffer, &size);
    if (stbuff == NULL) {
        perror("open_memstream");
        exit(EXIT_FAILURE);
    }

    printf("Enter commands (type 'quit' to finish):\n");

    for (;;) {
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;  // EOF

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "quit") == 0)
            break;

        fprintf(stbuff, "%s\n", line);
        fflush(stbuff);
    }

    fpout = fopen("script.sh", "w");
    if (fpout == NULL) {
        perror("fopen");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    fprintf(fpout, "#!/bin/bash\n");
    fprintf(fpout, "%s", buffer);
    fclose(fpout);
    free(buffer);

    chmod("script.sh", 0755);

    printf("\nExecute the script? (y/n) ");
    if (fgets(s, sizeof(s), stdin) != NULL) {
        if (s[0] == 'y' || s[0] == 'Y') {
            printf("\nExecuting script.sh...\n");
            char *args[] = {"./script.sh", NULL};
            execve("./script.sh", args, environ);
            perror("execve");  
        }
    }

    return 0;
}
