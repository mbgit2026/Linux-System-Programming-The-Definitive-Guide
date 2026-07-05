#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char *argv[])
{
	int fd, offset;
	char buffer[64];
	size_t size;

	fd = open("./file.txt", O_RDONLY);

	printf("\nInsert the offset: ");
	scanf("%d", &offset);

	if (( size = pread(fd, buffer, 64, offset)) == -1) {
		perror("pread");
		return 1;
	}

	printf ("\nFile contents: \n%s", buffer);
	close (fd);
	return 0;
}

	


	

