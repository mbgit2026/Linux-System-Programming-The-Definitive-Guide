#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int rfd, wfd;
	int rflags, wflags, perm;

	rflags = O_RDONLY;
	wflags = O_CREAT | O_WRONLY | O_TRUNC;
	perm = S_IRUSR | S_IWUSR;

	rfd = open ("file.txt", rflags);

	if (rfd == -1)
		perror("open");
	else
		printf ("\nfile.txt open in read mode");

	wfd = open ("file2.txt", wflags, perm);

	if (wfd == -1)
		perror ("open");
	else
		printf ("\nfile2.txt open/created in write mode (truncate to 0)");

	return 0;
}
