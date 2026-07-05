#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main (int argc, char *argv)
{
	int fd, n, len;
	char string[64];

	fd = open ("./file.txt", O_WRONLY | O_CREAT | O_TRUNC) ;
	if (fd == -1)
		perror ("open");

	while ((n = read (STDIN_FILENO, string, 64)) != 0) {
		if (n != 64) 
			string[n] = '\0';

		len = strlen(string);
		write (fd, string, len);
	}

	return 0;
}
