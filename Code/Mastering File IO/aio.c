#include <stdio.h>
#include <aio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

struct aiocb Request (int fd, off_t offset, void * content, size_t len) 
{
	struct aiocb aio;
	memset(&aio, 0, sizeof(aio));
	aio.aio_fildes = fd;
	aio.aio_offset = offset;
	aio.aio_buf = content;
	aio.aio_nbytes = len;
	aio.aio_lio_opcode = LIO_WRITE; 
	return aio;
}

void check_error(const char *label, struct aiocb *aio) {
	int err = aio_error(aio);
	if (err == 0) {
    	printf("%s completed successfully\n", label);
	} else if (err == EINPROGRESS) {
	    printf("%s still in progress\n", label);
	} else {
    	printf("%s failed: %s\n", label, strerror(err));
	}
}

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf("\nUsage: %s <file>", argv[0]);
		return 1;
	}

	const struct aiocb * op[2];
	int fd, ret;
	char str[6] = "Linux\0";
	char str2[13] = " Programming\0";

	fd = open (argv[1], O_RDWR);
	if (fd == -1)
		perror ("open");

	struct aiocb op1 = Request (fd, 0, str, strlen(str));
	ret = aio_write(&op1);
	if (ret == -1) 
		perror("aio_write");

	struct aiocb op2 = Request (fd, strlen(str), str2, strlen(str2));
	ret = aio_write (&op2);
	if (ret == -1) 
		perror("aio_write");


	op[0] = &op1;
	op[1] = &op2;

	if (aio_suspend(op, 2, NULL) == -1) 
	    perror("aio_suspend");
	

	ssize_t w1 = aio_return(&op1);
	ssize_t w2 = aio_return(&op2);
	printf("op1 wrote %zd bytes\n", w1);
	printf("op2 wrote %zd bytes\n", w2);

	check_error("op1", &op1);
	check_error("op2" ,&op2);
	return 0;
}
	
