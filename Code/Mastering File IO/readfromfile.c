#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define COUNT 128

int main (int argc, char *argv[])
{
	int fd;
	ssize_t n;
	char buffer[COUNT];

	fd = open ("./file.txt", O_RDONLY);

	if (fd < 0) {
		printf ("Errore open - exit..");
		return 1;
	}

	n = read (fd, buffer, COUNT);
	if (n < 0) {
    	printf("Read error\n");
    	close(fd);
    	return 1;
	}

	buffer[n] = '\0';

	printf ("Bytes read: %d", n);
	printf ("\nContent: %s", buffer);

	close (fd);
	return 0;
}
	
