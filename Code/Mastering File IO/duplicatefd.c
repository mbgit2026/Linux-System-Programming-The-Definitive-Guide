#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{

	if (argc != 4) {
		fprintf(stderr, "\nUsage: %s <filename> <string1> <string2>", argv[0]);
		return 1;
	}

	int fd, fd2;
	char *string1 = argv[2];
	char *string2 =  argv[3];

	fd = open (argv[1], O_CREAT | O_RDWR | O_TRUNC, 0640);

	if (write(fd, string1, strlen(string1)) == -1) {
		perror("write");
		return 1;
	}

	fd2 = dup (fd);
	if (write(fd2, string2, strlen(string2)) == -1) {
		perror("write");
		return 1;
	}

	

	return 0;
}
