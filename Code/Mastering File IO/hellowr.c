#include <fcntl.h>
#include <unistd.h>

int main()
{
	int fd;

	fd = open ("/dev/stdout", O_RDWR);
	write (fd, "hello", 5);

	return 0;
}			                   