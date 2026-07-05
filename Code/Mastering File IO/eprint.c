#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void eprintf (const char *template, ...)
{
	va_list ap;
	va_start (ap, template);
	vprintf (template, ap);
	va_end(ap);
}

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("\nUsage: %s file", argv[0]);
		return 1;
	}

	FILE *fp;
	const char *template;
	char buffer[BUFSIZ];

	fp = fopen (argv[1], "r");

	if (fp == NULL) {
		template = "\nError opening the file: %s";
		eprintf (template, argv[1]);
		return 1;
	} else {
		template = "\nFile: %s open successfully";
		eprintf (template, argv[1]);
	}

	fgets (buffer, BUFSIZ, fp);

	template = "\nReading file: %s - Bytes read %d \nData: %s";
	eprintf (template, argv[1], strlen(buffer), buffer);

	return 0;
}


