#include <stdio.h>

int main()
{
FILE *file;
char c;

file = fopen("./file.txt", "r");

while (c!=EOF)  {
	c=fgetc(file);
	if(c!=EOF)
		fputc(c, stdout);
}
fclose(file);
return 0;
}
