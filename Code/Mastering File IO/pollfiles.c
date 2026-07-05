#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    int nfds = argc - 1;          
    int open_nfds = nfds;         
    int timeout = 5000;           

    struct pollfd *fds = calloc(nfds, sizeof(struct pollfd));
    if (!fds) {
        perror("calloc");
        return 1;
    }

    for (int i = 0; i < nfds; i++) {
        fds[i].fd = open(argv[i + 1], O_RDONLY);
        if (fds[i].fd < 0) {
            fprintf(stderr, "Error opening %s: %s\n", argv[i + 1], strerror(errno));
            fds[i].fd = -1;  
            open_nfds--;
            continue;
        }
        fds[i].events = POLLIN;
    }

    while (open_nfds > 0) {
        int ret = poll(fds, nfds, timeout);
        if (ret == -1) {
            perror("poll");
            break;
        }
        if (ret == 0) {
            printf("Timeout expired, no activity.\n");
            continue;
        }

        for (int i = 0; i < nfds; i++) {
            if (fds[i].fd == -1)
                continue;

            if (fds[i].revents & POLLIN) {
                char buffer[1024];
                ssize_t n = read(fds[i].fd, buffer, sizeof(buffer));
                printf("[File %d: %s]\n%.*s\n", i + 1, argv[i + 1], (int)n, buffer);        
                sleep(1);
            } else {
                fprintf(stderr, "Read error on %s: %s\n",
                        argv[i + 1], strerror(errno));
                close(fds[i].fd);
                fds[i].fd = -1;
                open_nfds--;
            }
            
        }
    }

    free(fds);
    return 0;
}
