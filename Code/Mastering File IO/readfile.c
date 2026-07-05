#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define COUNT 128

int main()
{
	int fd;
	ssize_t n;
	char buffer[256];

	fd = open ("./file.txt", O_RDONLY);
	if (fd < 0) {
		printf ("Error opening file - exit..");
		return 1;
	}

	while ((n = read(fd, buffer, COUNT)) > 0) {
 	   if (write(STDOUT_FILENO, buffer, n) != n) {
    	    printf("Write error\n");
        	close(fd);
        	return 1;
    	}
	}

	if (n < 0) {
    	printf("Read error\n");
    	close(fd);
    	return 1;
	}
	
	close (fd);
	return 0;
}


