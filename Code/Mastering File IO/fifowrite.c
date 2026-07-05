#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("\nUsage: %s <string>", argv[0]);
		return 1;
	}

	int fd, n;

	fd = open ("./myfifo", O_WRONLY);
	write (fd, argv[1], strlen(argv[1]));

	return 0;
}
