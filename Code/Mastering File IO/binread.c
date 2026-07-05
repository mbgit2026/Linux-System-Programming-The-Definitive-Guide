#include <stdio.h>

#define BUFF_SIZE 8

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf ("\nUsage: %s <file>", argv[0]);
		return 1;
	}

	char chars[BUFF_SIZE];
	FILE *fp;
	size_t n;
	int i;

	fp = fopen (argv[1], "r");
	if (fp == NULL)
		perror ("fopen");

	n = fread (chars, sizeof(char), BUFF_SIZE, fp);
	if (n != BUFF_SIZE) 
		perror("read");

	printf ("\nDecimal        Hex        ASCII\n");
	for (i=0; i<BUFF_SIZE; i++) 
		printf ("\n%d\t\t%x\t    %c", chars[i], chars[i], chars[i]);

	fclose (fp);
	return 0;
}

