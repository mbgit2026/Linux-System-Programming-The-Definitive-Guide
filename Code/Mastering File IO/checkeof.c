#include <stdio.h>

int main ()
{
	FILE *fp;
	char buffer[100];
	
	fp = fopen ("file.txt", "r");

	while (!feof(fp)) {
		fread (buffer, sizeof(char), 100, fp);
		if (ferror(fp))
			printf ("Read error");
		printf ("%s", buffer);
	}

	printf ("\nEOF = %d", feof(fp));
	printf("\nClear the indicators...\n");
	clearerr(fp);
	printf ("\nEOF = %d", feof(fp));

	return 0;
}
