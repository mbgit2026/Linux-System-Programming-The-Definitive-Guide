#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char *argv)
{
	int fd;
	char stringa[24] = "Write with Linux..\0";

	fd = open ("./file.txt", O_WRONLY);
	if (fd == -1)
		perror ("write");

	write (fd, stringa, 24);

	return 0;
}
