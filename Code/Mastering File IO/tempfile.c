#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	int fd;
	char template[]="/tmp/fileXXXXXX";

	fd = mkstemp(template);

	if (fd == -1)
		perror ("mkstemp");

	printf ("\nFilename %s", template);

	close (fd);
	//unlink (template);
	return 0;
}


