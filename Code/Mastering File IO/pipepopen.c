#include <stdio.h>
#include <stdlib.h>

#define PAGER "/bin/more"

int main (int argc, char *argv[])
{
	if (argc != 2) {
		fprintf (stderr, "Usarg: %s <file>", argv[0]);
		exit (EXIT_FAILURE);
	}

	char line[1024];
	FILE *fpin, *fpout;

	fpin = fopen (argv[1], "r");
	if (fpin == NULL)
		perror ("fopen");

	fpout = popen (PAGER, "w");
	if (fpout == NULL)
		printf ("Errore popen");

	while (fgets(line, 1024, fpin) != NULL) {
		if (fputs (line, fpout) == EOF) 
			fprintf (stderr, "Error writing on the pipe");
	}

	if (pclose (fpout) == -1)
		fprintf (stderr, "Errore pclose");

	exit (EXIT_SUCCESS);
}


