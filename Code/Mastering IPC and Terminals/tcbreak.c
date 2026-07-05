#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd;
    const char *msg = "Sending data to terminal...\n";

    // Open controlling terminal (or any tty device)
    fd = open("/dev/tty", O_WRONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Write some data (goes into output queue)
    write(fd, msg, strlen(msg));

    printf("Data written. Waiting for output to drain...\n");

    // Ensure all queued output is transmitted
    if (tcdrain(fd) == -1) {
        perror("tcdrain");
        return 1;
    }

    printf("Output fully transmitted.\n");

    // Send a break condition on the terminal line
    printf("Sending break signal...\n");
    if (tcsendbreak(fd, 0) == -1) {
        perror("tcsendbreak");
        return 1;
    }

    printf("Break sent.\n");

    close(fd);
    return 0;
}