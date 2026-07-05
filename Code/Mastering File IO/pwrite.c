#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{

	if (argc != 4) {
		printf("\nUsage: %s <file> <offset> <data>", argv[0]);
		return 1;
	}

	int fd, offset = atoi(argv[2]);
	char *buffer = argv[3];
	size_t size;

	fd = open (argv[1], O_RDWR | O_CREAT, 0644);

	if ((size = pwrite(fd, buffer, strlen(buffer), offset)) == -1) {
		perror("write");
		return 1;
	} 
	
	close (fd);
	return 0;
}

	


	

