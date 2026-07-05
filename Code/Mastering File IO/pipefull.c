#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char *argv[])
{
	int fd1[2], fd2[2];
	pid_t pid;
	char buffer[5];

	pipe (fd1);
	pipe (fd2);

	pid = fork();

	if (pid > 0) {
		write (fd1[1], "Linux", 5);
		read (fd2[0], buffer, 5);
		printf ("\nFrom child: %s", buffer);
		wait(NULL);
	}
	else if (pid == 0) {
		read (fd1[0], buffer, 5);
		printf ("\nFrom parent: %s", buffer);
		write (fd2[1], "Hello", 5);
	}

	return 0;
}



	
