#include <stdio.h>

#define BUF 128

int main (int argc, char *argv[])
{
	FILE *fp;
	char c;
	char buffer[BUF];
	
	fp = tmpfile();
	printf("\nEnter an empty line to exit\n\n");

	while (fgets (buffer, BUF, stdin)) {
		if (*buffer == '\n')
			break;
		fputs (buffer, fp);
	}

	printf ("\nSave the data (y/n): ");
	scanf (" %c", &c);

	if (c == 'y'){
		FILE *fpout = fopen ("file.out", "w");
		fseek(fp, 0, SEEK_SET);

		while (fgets(buffer, BUF, fp)) 
			fputs (buffer, fpout);

		printf("\nData saved");
		fclose(fp);
		fclose(fpout);
	}

	return 0;
}


		


