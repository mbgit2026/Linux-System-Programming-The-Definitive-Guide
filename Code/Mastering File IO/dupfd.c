#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <filename> <fd_num>", argv[0]);
		return 1;
	}

	int fdnew = atoi(argv[2]);
	char buffer[7];
	int fd = open (argv[1], O_RDWR | O_CREAT | O_TRUNC, 0640);

	if (write (fd, "old", 3) == -1)
		perror ("write");

	if (dup2(fd, fdnew) == -1)
		perror ("dup2");

	if (write (fdnew, " new", 4) == -1)
		perror ("write");

	lseek (fdnew, 0, SEEK_SET);
	read (fdnew, buffer, 7);
	printf ("%s", buffer);
	return 0;
}
