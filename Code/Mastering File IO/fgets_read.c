#include <stdio.h>

int main()
{
	FILE *file;
	char buffer[1024];
	size_t readed = 1024;

	if ((file = fopen ("Topolino.txt", "r")) == NULL)
	{
		printf("\n Error opening file \n");
		return 1;
	}

	while (fgets (buffer, readed, file))
		fputs(buffer, stdout);

	fclose (file);

	return 0;
}	

