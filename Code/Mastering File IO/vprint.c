#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void print(const char *template, ...)
{
	va_list ap;
	va_start(ap, template);
	vprintf (template, ap);
	va_end(ap);
}

int main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("\nUsage: %s <number>", argv[0]);
		return 1;
	}

	int a = atoi(argv[1]);
	const char *template = "Number = %d";
	print(template, a);
	return 0;
}

