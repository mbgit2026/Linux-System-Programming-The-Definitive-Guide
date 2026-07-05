#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("\nUsage %s <filename>", argv[0]);
		return 1;
	}

	FILE *fp;
	char *filename = argv[1];
	char buffer[32];

	if ((fp = fopen(filename, "r")) == NULL)
		perror ("fopen");

	printf("\n- File open in read-only mode -");
	printf ("\n- Read from fp -\n\n");
	while ((fgets (buffer, 32, fp)) != NULL)
		fputs (buffer, stdout);

	printf("\n\n- Change file mode to append mode.. -");
	if ((fp = freopen (NULL, "a", fp)) == NULL)
		perror ("freopen");

	
	printf("\n- Write on file... -");
	if (fputs("is interesting..", fp) == EOF)
		perror("fputs");

	printf("\n- File updated -");

	fclose(fp);
	return 0;
}



	
