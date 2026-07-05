#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file> <block | noblock>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
	int flags;

    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("Trying to acquire exclusive lock on %s...\n", filename);

	if (strcmp(argv[2], "block") == 0)
		flags = LOCK_EX;
	else if (strcmp(argv[2], "noblock") == 0)
		flags = LOCK_EX | LOCK_NB;

    if (flock(fd, flags) == -1) {
        perror("flock");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Exclusive lock acquired! (PID: %d)\n", getpid());
    printf("Press Enter to release the lock and exit...\n");
    getchar();

    if (flock(fd, LOCK_UN) == -1) {
        perror("flock unlock");
    }

    close(fd);
    return 0;
}


