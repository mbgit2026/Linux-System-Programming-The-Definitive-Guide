#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

int dirfd;

void handler(int sig) {
    if (sig == SIGIO) {
        printf("Directory change detected!\n");
        fcntl(dirfd, F_NOTIFY, DN_CREATE | DN_DELETE | DN_RENAME | DN_MODIFY);
    }
}

int main() {
    const char *dirname = ".";  
    

    dirfd = open(dirname, O_RDONLY | O_DIRECTORY);
    if (dirfd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (fcntl(dirfd, F_SETOWN, getpid()) == -1) {
        perror("fcntl F_SETOWN");
        exit(EXIT_FAILURE);
    }

    if (fcntl(dirfd, F_SETFL, O_ASYNC) == -1) {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }

    if (fcntl(dirfd, F_NOTIFY, DN_CREATE | DN_DELETE | DN_RENAME | DN_MODIFY) == -1) {
        perror("fcntl F_NOTIFY");
        exit(EXIT_FAILURE);
    }

    printf("Watching directory '%s' for changes...\n", dirname);

    for (;;) 
        pause();
    

    close(dirfd);
    return 0;
}
