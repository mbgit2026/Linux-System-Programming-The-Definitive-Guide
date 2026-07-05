#include <stdio.h>
#include <stdarg.h>

void eprint (const char *template, ...)
{
	va_list ap;
	va_start (ap, template);
	vprintf (template, ap);
	va_end(ap);
}

void eread (const char *template, ...)
{
	va_list ap;
	va_start (ap, template);
	vscanf (template, ap);
	va_end(ap);
}


int main (int argc, char *argv[])
{
	int a,b,c;

	printf ("\nInsert 3 numbers: ");
	eread ("%d %d %d", &a, &b, &c);

	eprint("\nYou enetered: %d %d %d", a,b,c);
	return 0;
}





