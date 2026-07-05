#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#define SHELL "/bin/bash"

int main (int argc, char *argv[])
{
	int n, fd1[2], fd2[2];
	pid_t pid;
	char line[BUFSIZ];
	
	if (pipe (fd1) < 0)
		perror ("pipe");
	if (pipe (fd2) < 0)
		perror ("pipe");

	pid = fork();
	if (pid < 0) 
		perror ("fork");
	else if (pid > 0) {
		close (fd1[0]);
		close (fd2[1]);

		for (;;) {
			printf ("\ncommand>");
			fgets (line, BUFSIZ, stdin);
			n = strlen(line);
			if (write (fd1[1], line, n) != n)
				perror ("write");
			if ((n = read (fd2[0], line, BUFSIZ)) < 0)
				perror ("read");
			if (n == 0) {
				printf ("\nChild has closed the pipe");
				break;
			}
			line[n] = 0;
			if (fputs (line, stdout) == EOF)
				perror ("fputs");
		}
		exit (0);
	} else {
		close (fd1[1]);
		close (fd2[0]);
		if (dup2 (fd1[0], STDIN_FILENO) == -1)
			perror ("dup2");
		close (fd1[0]);
		if (dup2 (fd2[1], STDOUT_FILENO) == -1)
			perror ("dup2");
		close (fd2[1]);
		if (execl (SHELL, SHELL, NULL) < 0)
			perror ("execl");
	}

	return 0;
}

