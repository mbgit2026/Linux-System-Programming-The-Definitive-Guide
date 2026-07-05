#include <stdio.h>

int main()
{
	FILE *file;
	char c;

	if ((file = fopen ("./file.txt", "w+")) == NULL) {
		perror("fopen");
		return 1;
	}

	while ((c = fgetc (stdin)) != EOF)
		fputc(c, file);

	fclose (file);
	return 0;
}	

