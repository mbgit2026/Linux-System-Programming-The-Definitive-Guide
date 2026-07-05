#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[])
{

	if (argc != 2) {
		printf("\nUsage: %s <file>", argv[0]);
		return 1;
	}

	FILE *fp;
	fpos_t pos;
	char buffer[64];

	if ((fp = fopen (argv[1], "r+")) == NULL)
		perror ("fopen");

	while (strcmp("quit", buffer) != 0) {
		rewind(fp);
		fgetpos(fp, &pos);
		printf("\n\nCurrent position: %ld", pos);
		printf("\nInsert data (quit to exit): ");
		scanf ("%s", &buffer);
		if ((strcmp ("quit", buffer)) == 0) 
			return 0;
		fputs (buffer, fp);		
		printf("\nData written in the file: %s", buffer);
		fgetpos(fp, &pos);
		printf("\nCurrent position: %ld", pos);
	}
			
	return 0;
}
	

