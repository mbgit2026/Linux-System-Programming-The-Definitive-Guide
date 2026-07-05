#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char path[64];
    snprintf(path, sizeof(path), "/proc/%s/fd", argv[1]);

    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    printf("File descriptors for PID %s:\n", argv[1]);
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;
        printf("  FD: %s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}
