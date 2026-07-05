#include <stdio.h>

#define BUFF_SIZE 8192

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf ("\nUsage: %s <file>", argv[0]);
		return 1;
	}

	char chars[BUFF_SIZE];
	FILE *fpin, *fpout;
	size_t n;
	int i;

	fpin = fopen (argv[1], "r");
	fpout = fopen ("copy_of_file.bin", "w");

	while (n = fread (chars, sizeof(char), sizeof(*chars), fpin) != 0) 
		n = fwrite (chars, sizeof(char), sizeof(*chars), fpout);
	

	fclose (fpin);
	fclose (fpout);
	return 0;
}

