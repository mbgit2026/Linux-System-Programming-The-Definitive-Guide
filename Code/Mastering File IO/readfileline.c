#include <stdio.h>

int main()
{
	FILE *file;
	char buffer[1024];
	int count = 1024;

	if ((file = fopen ("./file.txt", "r")) == NULL) {
		printf ("Error opening file");
		return 1;
	}

	while (fgets (buffer, count, file))
		fputs (buffer, stdout);

	fclose (file);
	return 0;
}
