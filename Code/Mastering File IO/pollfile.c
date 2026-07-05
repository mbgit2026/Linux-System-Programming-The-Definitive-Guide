#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	struct pollfd *fds;
	int timeout = 1200;
	int ret, i, nfds, open_nfds;
	ssize_t readed;

	if (argc != 2) {
		printf ("nUsage: %s <file> ", argv[0]);
		return 1;
	}

	nfds = open_nfds = argc -1;
	fds = calloc(nfds, sizeof(struct pollfd));
	if (fds == NULL) {
		perror("calloc");
		return 1;
	}

	fds[0].fd = open (argv[1], O_RDONLY);
	if (fds[0].fd < 0) {
		perror("open");
		return 1;
	}
	
	fds[0].events = POLLIN;
	while (open_nfds > 0) {
		ret = poll(fds, nfds, timeout);
		if (ret == -1) {
			perror ("poll");
			return 1;
		} else if (ret == 0) {
            printf("Timeout expired...\n");
            continue;
        }

		for (i=0; i < nfds; i++) {
			char buffer[1024];
			if (fds[i].revents != 0) {
				if (fds[i].revents & POLLIN) {
					readed = read (fds[i].fd, buffer, sizeof(buffer));
					printf ("%.*s", (int) readed, buffer);
				} else {
					perror("read");
					close (fds[i].fd);
					open_nfds--;
				}
			}
		}
	}
	free(fds);
	return 0;
}

