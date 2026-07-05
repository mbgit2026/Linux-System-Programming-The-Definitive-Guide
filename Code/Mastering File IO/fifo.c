#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main ()
{
	int fd, n;
	char buffer[BUFSIZ];

	mkfifo ("./myfifo", S_IRWXU);

	fd = open ("./myfifo", O_RDONLY);

	while (read (fd, buffer, BUFSIZ) != 0) {
		n = strlen(buffer);
		buffer[n] = 0;
		write (STDOUT_FILENO, buffer, n);
	}

	return 0;
}
