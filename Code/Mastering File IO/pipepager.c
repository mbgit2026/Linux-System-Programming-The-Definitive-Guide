#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define PAGER "/bin/more"

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("\nUsage: %s <file>", argv[0]);
		exit (EXIT_FAILURE);
	}
	
	int fd[2];
	int n;
	pid_t pid;
	char line[1024];
	FILE *fp;

	fp = fopen (argv[1], "r");
	if (fp == NULL)
		perror ("fopen");

	if (pipe(fd) < 0)
		perror ("pipe");

	if ((pid = fork()) < 0) {
		perror ("fork");
	} else if (pid > 0) {
		close (fd[0]);
		while (fgets(line, 1024, fp) != NULL) {
			n = strlen (line);
			if (write(fd[1], line, n) != n)
				fprintf (stderr, "Error writing to pipe");
		}
		close (fd[1]);
		wait(NULL);
		exit (EXIT_SUCCESS);
	} else {
		close (fd[1]);
		if (fd[0] != STDIN_FILENO) {
			dup2 (fd[0], STDIN_FILENO);
		close (fd[0]);    
		}
		execl (PAGER, PAGER, NULL);
	}
	exit (EXIT_SUCCESS);
}
		
