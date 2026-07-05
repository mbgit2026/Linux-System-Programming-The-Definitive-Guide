#include <stdio.h>

// unget.c

int main ()
{
	char a,b,c,d,e,f,g;
	FILE *fp;

	fp = fopen ("file.txt", "r");

	a = getc(fp);
	printf ("\n1 = %c", a);

	b = ungetc(b, fp);

	c = getc(fp);
	printf ("\n2 = %c", c);

	d = getc(fp);
	printf ("\n3 = %c", d);

	e = getc(fp);
	printf ("\n4 = %c", e);

	f = getc(fp);
	printf ("\n5 = %c", f);

	g = ungetc ('z', fp);

	g = getc (fp);
	printf ("\n6 = %c", g);

	char h = getc(fp);
	printf ("\n7 = %c", h);

	return 0;
}

