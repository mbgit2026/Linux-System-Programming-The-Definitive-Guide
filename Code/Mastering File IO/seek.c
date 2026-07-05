#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 128

int main()
{
	int fd, n, offset;
	char buffer[SIZE] = "lseek allows to position in the file\0";
	char buffer2[SIZE];

	fd = open ("./file.txt", O_RDWR);
	if (fd == -1)
		perror ("open");

	n = write (fd, buffer, SIZE);

	printf ("\nEnter the offset: ");
	scanf (" %d", &offset);
	lseek (fd, offset, SEEK_SET);
	
	n = read (fd, buffer2, SIZE);
	printf ("\n%s \n", buffer2);
			
	close (fd);
	return 0;
}

