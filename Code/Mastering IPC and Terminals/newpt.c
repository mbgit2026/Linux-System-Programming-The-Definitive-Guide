#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>

int open_pty_pair(int *amaster, int *aslave) {
    int master, slave;
    char *name;

    master = posix_openpt(O_RDWR | O_NOCTTY);
    if (master < 0)
        return -1;

    if (grantpt(master) < 0 || unlockpt(master) < 0) {
        close(master);
        return -1;
    }

    name = ptsname(master);
    if (name == NULL) {
        close(master);
        return -1;
    }

    slave = open(name, O_RDWR);
    if (slave == -1) {
        close(master);
        return -1;
    }

    *amaster = master;
    *aslave = slave;
    return 0;
}

int main(int argc, char *argv[]) {
    int master, slave;
    pid_t pid;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (open_pty_pair(&master, &slave) == -1) {
        perror("open_pty_pair");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // CHILD

        close(master);

        if (setsid() < 0) {
            perror("setsid");
            exit(EXIT_FAILURE);
        }

        // Make slave the controlling terminal
        if (ioctl(slave, TIOCSCTTY, 0) < 0) {
            perror("ioctl TIOCSCTTY");
            exit(EXIT_FAILURE);
        }

        dup2(slave, STDIN_FILENO);
        dup2(slave, STDOUT_FILENO);
        dup2(slave, STDERR_FILENO);

        if (slave > 2)
            close(slave);

        execvp(argv[1], &argv[1]);

        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // PARENT
    close(slave);

    printf("child pid: %d\n", pid);
    printf("master fd: %d\n", master);

    char buf[256];
    ssize_t n;

    while ((n = read(master, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, n);
    }
 
    close(master);
    return 0;
}