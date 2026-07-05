#include <stdio.h>
#include <stdarg.h>

void foo(char *fmt, ...)
{
	va_list ap;
        int d;
        char c;
        char *s;

        va_start(ap, fmt);
        while (*fmt)
	        switch (*fmt++) {
		case 's':  
         	        s = va_arg(ap, char *);
         	        printf("string %s\n", s);
               	        break;
                case 'd':  
                        d = va_arg(ap, int);
                        printf("int %d\n", d);
                        break;
                case 'c':   
                        c = (char) va_arg(ap, int);
                        printf("char %c\n", c);
                        break;
               }
	va_end(ap);
}

int main (int argc, char *argv[])
{
	int d=4;
	char c='a';	
        char s[6] = "hello\0";
	char *template = "%c %d %s";
	foo (template, c, d, s);
	return 0;
}

