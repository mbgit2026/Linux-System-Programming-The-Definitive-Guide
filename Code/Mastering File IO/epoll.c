#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>

#define BUFSIZE 64

int main (int argc, char *argv[])
{
	int efd = epoll_create(1);
	struct epoll_event event;
	char buffer[BUFSIZE];
	size_t readed;

	event.events = EPOLLIN;
	event.data.fd = 0;

	epoll_ctl (efd, EPOLL_CTL_ADD, 0, &event);
	epoll_wait (efd, &event, 1, 30000);

	while (1==1)
	{
		readed = read (event.data.fd, buffer, BUFSIZE);
		buffer[readed] = '\0';
		printf ("\nDescriptor %d - %d bytes read", event.data.fd, readed);
		printf ("\nData: %s\n", buffer);

		if (strncmp(buffer, "quit\n", 5) == 0)
			break; 
	}

	close (efd);
	return 0;
}
