#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 1024

int main()
{
	int fd, n, c;
	char buffer[SIZE];

	fd = open ("file.txt", O_RDONLY);

	if (fd == -1) {
		if (errno == ENOENT) {
			fprintf (stderr,"Error: File not found");
			_exit(1);
		}
	}

	n = read (fd, buffer, SIZE);
	if (n == -1) {
		if (errno = EPERM) {
			fprintf (stderr,"Error: Operation not permitted");
			_exit(1);
		}
	}

	c = close(fd);
	if (c == -1) 
		perror ("close");

	return 0;
}


