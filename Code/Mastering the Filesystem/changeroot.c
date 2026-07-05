#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <new_root_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *new_root = argv[1];

    char cwd[1024];
    syscall(SYS_pivot_root, new_root, cwd);
        
    if (chdir("/") != 0) {
        perror("chdir");
        return EXIT_FAILURE;
    }

    printf("\nchroot successful. Running shell inside new root...\n");

    execl("/bin/sh", "/bin/sh", (char *)NULL);
    perror("execl");

    return EXIT_FAILURE;
}
