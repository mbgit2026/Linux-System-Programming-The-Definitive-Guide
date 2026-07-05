#include <stdio.h>

int
main (int argc, char *argv[])
{
	int a = 4;
	float b = 3.14;
	char c = 'a';
	char *s = "Linux";

	printf ("\n Integer: %d", a);
	printf ("\n signed integer: %+d %+d", a, -4);

	/* Il # mette il prefisso 0 ai valori ottali e 0x e 0X ai valori esadecimali */
	printf ("\n Octal: %#o", 1234);
	printf ("\n Hexadecimal: %#x", 1234);
	printf ("\n Hexadecimal capital: %#X\n", 1234);
	printf ("\n Floaring point: %f", b);
	printf ("\n Floating point exponential notation: %e", b);
	printf ("\n Character: %c", c);
	printf ("\n String: %s", s);
	printf ("\n String address: %p\n", &s);

	printf ("\n Floating point 3.1415167: 3 digits after . : %.3f",3.1415167);
	printf ("\n First 4 char of the Linux string: %.4s", s);

	printf ("\n\n Justify to right up to 4 chars");
	printf ("\n %4d", 1);
	printf ("\n %4d", 12);
	printf ("\n %4d", 123);
	printf ("\n %4d\n", 1234);

	printf ("\n Field boundary: 10\n");
	printf ("\n%10s%10d%10c%10f", s, a, c, b);
	printf ("\n\n Justify to left\n");
	printf ("\n%-10s%-10d%-10c%-10f\n", s, a, c, b);

	printf ("\n\n Field of size 9 padded with zeros\n");
	printf ("\n %+09d", 123);
	printf ("\n %09d", 123);

}

