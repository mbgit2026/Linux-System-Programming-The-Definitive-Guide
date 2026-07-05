#include <stdio.h>

// unget2.c

int main () 
{
	FILE *fp;
	char c;

	fp = fopen ("file.txt", "r");
	if (fp == NULL) {
		perror ("fopen");
		return 1;
	}

	while ((c=getc(fp)) != EOF) {
		if (c == '#') 
			ungetc('@', fp);
		else 
			ungetc(c, fp);
		
		c=getc(fp);
		putc (c, stdout);
	}

	return 0;
}
