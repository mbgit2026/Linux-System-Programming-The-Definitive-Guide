#include <stdio.h>
#include <aio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

struct aiocb Request (int fd, off_t offset, void * content, size_t len) 
{
	struct aiocb aio;
	aio.aio_fildes = fd;
	aio.aio_offset = offset;
	aio.aio_buf = content;
	aio.aio_nbytes = len;
	aio.aio_reqprio = 0;
	aio.aio_sigevent.sigev_notify = SIGEV_NONE;
	aio.aio_lio_opcode = LIO_WRITE;
	return aio;
}

int main (int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "\nUsage: %s <file>", argv[0]);
		return 1;
	}

	struct aiocb * op[2];
	int fd;
	char *str = "Asynchronous";
	char *str2 = "Requests";

	fd = open (argv[1], O_RDWR);
	if (fd == -1)
		perror ("open");

	struct aiocb op1 = Request (fd, 0, str, strlen(str));
	struct aiocb op2 = Request (fd, strlen(str), str2, strlen(str2));

	op[0] = &op1;
	op[1] = &op2;

	
	if (lio_listio(LIO_WAIT, op, 2, NULL) < 0)
		perror("listio");
	
	return 0;
}









		
