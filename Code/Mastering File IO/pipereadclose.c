#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main (int argc, char *argv[])
{
	if (argc < 2) {
		printf ("\nUsage %s <1|0>", argv[0]);
		exit (EXIT_FAILURE);
	}

	int pipefd[2];
	char buffer[5];
	int n;

	if (pipe(pipefd) == -1) {
		perror ("pipe");
		exit (EXIT_FAILURE);
	}

	signal (SIGPIPE, SIG_IGN);

	int co = atoi (argv[1]);
	if (co == 0) 
		close (pipefd[0]);
	else
		printf ("\nRead descriptor opened - continue..");
	
	n = write (pipefd[1], "linux", 5);
	if (n == -1)
		perror("write");

	n = read (pipefd[0], buffer, 5);
	if (n == -1)
		perror("read");
	else
		printf ("\nBuffer: %s", buffer);

	exit (EXIT_SUCCESS);
}
