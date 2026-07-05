#include <stdio.h>

int main()
{
	FILE *file;
	char c;

	if ((file = fopen ("./file.txt", "a+")) == NULL)
		perror("fopen"); 

	while ((c = fgetc (stdin)) != EOF)
		fputc (c, file);

	fseek (file, 0, 0);
	while ((c = fgetc (file)) != EOF)
			fputc (c, stdout);

	fclose (file);
	return 0;
}	

