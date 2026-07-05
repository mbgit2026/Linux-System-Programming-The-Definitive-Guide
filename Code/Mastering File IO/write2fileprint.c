#include <stdio.h>

int main()
{
	FILE *filein;
	FILE *fileout;
	char c;

	if ((filein = fopen ("./file.txt", "r")) == NULL) 
		perror("fopen");
		

	if ((fileout = fopen ("./file2.txt", "w+")) == NULL) 
		perror("fopen");

	while ((c = fgetc (filein)) != EOF)
		fputc (c, fileout);

    fseek(fileout, 0, SEEK_SET);

    while ((c = fgetc (fileout)) != EOF)
		fputc (c, stdout);


	fclose (filein);
	fclose (fileout);
	return 0;
}	
