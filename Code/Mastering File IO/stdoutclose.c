#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void main() 
{
	int fd;
	char buffer[6]="hello\0";

	//close(1);
	fd = open("file.txt", O_RDWR);
	printf (" %d", fd);
	write (fd, buffer, 6);	
}