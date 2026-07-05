#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// From linux/fs.h (in kernel headers)
#define EXT4_IOC_GETFLAGS       FS_IOC_GETFLAGS
#define EXT4_IOC_SETFLAGS       FS_IOC_SETFLAGS

#define EXT4_IMMUTABLE_FL       0x00000010  // Immutable file flag

void print_flags(int flags) {
    printf("EXT4 flags: 0x%08X\n", flags);
    if (flags & EXT4_IMMUTABLE_FL)
        printf(" - IMMUTABLE\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    const char *file = argv[1];
    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    int flags;

    // Get current flags
    if (ioctl(fd, EXT4_IOC_GETFLAGS, &flags) < 0) {
        perror("ioctl - GETFLAGS");
        close(fd);
        return 1;
    }

    printf("Current flags for %s:\n", file);
    print_flags(flags);

    // Set immutable flag if not set
    if (!(flags & EXT4_IMMUTABLE_FL)) {
        printf("Setting IMMUTABLE flag...\n");
        flags |= EXT4_IMMUTABLE_FL;

        // Need to reopen read/write to change flags
        close(fd);
        fd = open(file, O_RDWR);
        if (fd < 0) {
            perror("reopen for SETFLAGS");
            return 1;
        }

        if (ioctl(fd, EXT4_IOC_SETFLAGS, &flags) < 0) {
            perror("ioctl - SETFLAGS");
            close(fd);
            return 1;
        }

        printf("IMMUTABLE flag set successfully.\n");
    } else {
        printf("IMMUTABLE flag already set.\n");
    }

    close(fd);
    return 0;
}
