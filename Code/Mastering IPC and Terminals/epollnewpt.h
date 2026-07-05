#define _GNU_SOURCE
#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/epoll.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <syscall.h>


#ifndef SYS_pidfd_open
#define SYS_pidfd_open 434
#endif

#define MAX_EVENTS 15
#define BUF_SIZE 1024
#define MAX_CMD 64
#define MAX_ARGS 64




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


// Helper to parse input string into argv array
void parse_command(char *input, char **argv) {
    char *token = strtok(input, " \n");
    int i = 0;
    while (token != NULL) {
        argv[i++] = token;
        token = strtok(NULL, " \n");
    }
    argv[i] = NULL;
}
