#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("\nUsage: %s <filename>", argv[0]);
		return 1;
	}

	char *filename = argv[1];

	if (creat(filename, S_IRWXU) == -1 )
		perror("creat");


	return 0;
}
