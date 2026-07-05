#include <stdio.h>

int main()
{
	char buf[256];

	while (fgets(buf, 256, stdin) != NULL)
		fputs(buf, stdout);
}

