#define _GNU_SOURCE
#include <stdio.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("\nUsage: %s <file>", argv[0]);
		return 1;
	}

	int fdin, fdout;
	struct iovec iov[2];
	int readed=0, writed=0;

	struct stat st;
	stat(argv[1], &st);

	char *str1 = malloc(st.st_size / 2);
	char *str2 = malloc(st.st_size / 2);

	fdin = open (argv[1], O_RDONLY);
	if (fdin == -1) {
		perror ("open");
		return(1);
	}

	fdout = open ("file.out", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	if (fdout == -1) {
		perror ("open");
		return (1);
	}

	iov[0].iov_base=str1;
	iov[0].iov_len=st.st_size / 2;
	iov[1].iov_base=str2;
	iov[1].iov_len=st.st_size / 2;

	while ((readed = preadv2 (fdin, iov, 2, -1, RWF_NOWAIT)) > 0) {
		writed = pwritev2 (fdout, iov, 2, -1, RWF_APPEND);
	}
		
	close(fdin);
	close(fdout);
	return 0;
}

