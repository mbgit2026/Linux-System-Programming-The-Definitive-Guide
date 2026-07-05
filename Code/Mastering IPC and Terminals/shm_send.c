#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFSIZE 1024

struct shmbuf {
	size_t cnt;
	char buf[BUFSIZE];
};

 int main (int argc, char *argv[])
  {
	if (argc != 3) {
		printf ("\nUsage: %s: /shm-path string", argv[0]);
		return 1;
	}

	char *shmpath = argv[1];
	char *string = argv[2];
	size_t len = strlen (string);
	struct shmbuf *shmp;
	int fd;

	if (len > BUFSIZE) {
		printf ("\nString is too big - exit...");
		return 1;
	}

	fd = shm_open (shmpath, O_RDWR, 0);
	shmp=mmap(NULL, sizeof(*shmp), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	shmp->cnt = len;
	memcpy (&shmp->buf, string, len);

	sleep (20);
	
	write (STDOUT_FILENO, &shmp->buf, len);
	write (STDOUT_FILENO, "\n", 1);
	return 0;
  }