#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int fd, n, i, offset;
	char c, *str[6];

	fd = open ("./file.txt", O_RDWR);
	if (fd == -1)
		perror ("open");

	printf ("\nInsert the offset: ");
	scanf (" %d", &offset);
	lseek (fd, offset, SEEK_SET);

	
	printf ("\nInsert the string: ");
	scanf ("%5s", &str);

	n=write (fd, str, 5);
	
	close (fd);
	return 0;
}

