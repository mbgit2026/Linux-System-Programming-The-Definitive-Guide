#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void print(const char *template, ...)
{
	va_list ap;
	va_start(ap, template);
	vprintf (template, ap);
	va_end(ap);
}

int main (int argc, char *argv[])
{
	if (argc < 2) {
		printf ("\nUsage: %s <num1> [num2] [num3]", argv[0]);
		return 1;
	}

	int count = argc - 1;
	char template[64];
    int i;

	for (i=0; i<count; i++)
        strncat(template, "Number = %d\n", sizeof(template) - strlen(template) - 1);

    switch (count)
    {
        case 1:
            print(template, atoi(argv[1]));    
            break;
        case 2:
            print(template, atoi(argv[1]), atoi(argv[2]));    
            break;
        case 3:
            print(template, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));    
            break;
        
        default:
            break;
    }  

	return 0;
}