#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 128

int main()
{
	int fd, fdin;
	char buffer[SIZE];

	if ((fd=open ("./file.txt", O_CREAT|O_RDWR|O_TRUNC, S_IRWXU)) == -1)
		perror("open");

	if ((fdin=open ("/usr/share/doc/zsh/doc/examples/zshrc",O_RDONLY)) == -1)
		perror("open");
	
	while((read(fdin, buffer, SIZE)) > 0) {
		 write(fd, buffer, SIZE);
		 fsync(fd);
	}

	close (fdin);
	close (fd);
	return 0;
}
