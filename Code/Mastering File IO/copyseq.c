#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "\nUsage: %s <n>", argv[0]);
        exit (EXIT_FAILURE);
    }

    int numreq = atoi(argv[1]);
    int i;

    char files[numreq][16]; 
    for (i = 0; i < numreq; i++) {
        snprintf(files[i], sizeof(files[i]), "file%d", i+1);
    }

    int fds[numreq];
    for (i = 0; i<numreq; i++) {
        fds[i] = open(files[i], O_CREAT | O_WRONLY | O_DIRECT , 0644);
        if (fds[i] < 0) {
            perror("open");
            exit(1);
        }
    }

    void *msg;
    posix_memalign(&msg, 32768, 32768);  
    memset(msg, 'A', 32768);

    for (i=0; i<numreq; i++) {
        write(fds[i], msg, 32768);
    }

    for (i=0; i<numreq; i++)
        close(fds[i]);

    return 0;

}