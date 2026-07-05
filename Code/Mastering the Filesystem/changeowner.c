#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    
    uid_t uid;

    if (argc < 2) {
        printf("Usage: %s <UID> <filename>", argv[0]);
        exit (1);
    }

    uid = strtol(argv[1], NULL, 10);

    if (chown(argv[2], uid, -1) == -1) {
        perror("chown");
        exit(1);
    }

    exit(0);
}
