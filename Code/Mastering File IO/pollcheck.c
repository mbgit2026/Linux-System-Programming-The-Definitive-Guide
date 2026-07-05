#include <stdio.h>
#include <unistd.h>
#include <poll.h>

int main()
{
	struct pollfd fds[2];
	int ret, timeout = 5000;

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;
	fds[1].fd = STDOUT_FILENO;
	fds[1].events = POLLOUT;

	ret = poll (fds, 2, timeout);

	if (ret == -1) 
		perror ("poll");
		
	if (!ret) {
		printf ("Timeout %d secondi - exit..", timeout);
		return 0;
	}

	if (fds[0].revents & POLLIN)
		printf ("stdin is readable\n");

	if (fds[1].revents & POLLOUT)
		printf ("stdout is writable");

	return 0;
}


